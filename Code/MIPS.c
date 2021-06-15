#include "MIPS.h"
extern InterCodes code_root;
int avail_reg = 18;
int fp_offset = -4;
VarManager VarMan_head = NULL, VarMan_tail = NULL;

void mips(char *filename)
{
    FILE *f = fopen(filename, "w");
    if (f == NULL)
    {
        fprintf(stderr, "Output file open error\n");
    }

    init_print(f);
    instruction_select(f);
}

void init_print(FILE *f)
{
    fprintf(f, ".data\n");
    fprintf(f, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(f, "_ret: .asciiz \"\\n\"\n");
    fprintf(f, ".globl main\n");
    fprintf(f, ".text\n");
    fprintf(f, "read:\n");
    fprintf(f, "  li $v0, 4\n");
    fprintf(f, "  la $a0, _prompt\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  li $v0, 5\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  jr $ra\n");
    fprintf(f, "\n");
    fprintf(f, "write:\n");
    fprintf(f, "  li $v0, 1\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  li $v0, 4\n");
    fprintf(f, "  la $a0, _ret\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  move $v0, $0\n");
    fprintf(f, "  jr $ra\n");
}

VarManager var_find(char* name)
{  
    VarManager cur = VarMan_head;
    while(cur!=NULL)
    {
        Operand op = cur->var;
        if(0==strcmp(name, op->u.char_value))
            return cur;
        cur = cur->next;
    }
    return NULL;
}

void var_insert(Operand op)
{
    if(var_find(op->u.char_value)!=NULL)
        return;
    if(op->kind!=VARIABLE_OP)
        return;
    VarManager vm = (VarManager)malloc(sizeof(struct VarManager_));
    vm->var = op;
    vm->u.offset = fp_offset;
    fp_offset-=4;
    vm->next = NULL;
    if(VarMan_head == NULL)
    {
        VarMan_head = vm;
        VarMan_tail = vm;
    }
    else
    {
        VarMan_tail->next = vm;
        VarMan_tail = vm;
    }
}

void free_space()
{
    while(VarMan_head!=NULL)
    {
        VarManager tmp = VarMan_head;
        VarMan_head = VarMan_head->next;
        free(tmp);
    }
    VarMan_head = NULL;
    VarMan_tail = NULL;
}

void alloc_space(InterCodes func_head)
{
    int code_kind = 0;
    InterCode code = NULL; 
    while(func_head!=NULL&&func_head->code->kind!=FUNCTION_IR)
    {
        code = func_head->code;
        code_kind = func_head->code->kind;
        if(code_kind == ASSIGN_IR)
        {
            fprintf(stderr, "%s\n", code->u.assign.left->u.char_value);
            var_insert(code->u.assign.left);
            var_insert(code->u.assign.right);
        }
        else if(code_kind == ADD_IR||code_kind == SUB_IR||code_kind == MUL_IR||code_kind == DIV_IR)
        {
            var_insert(code->u.binop.result);
            var_insert(code->u.binop.op1);
            var_insert(code->u.binop.op2);
        }
        else if(code_kind == IF_IR)
        {
            var_insert(code->u.if_code.op1);
            var_insert(code->u.if_code.op2);
        }
        else if(code_kind == DEC_IR)
        {
            fp_offset += 4;
            var_insert(code->u.assign.left);
            fp_offset -= code->u.assign.right->u.int_value;
        }
        else if(code_kind ==  READ_IR||code_kind == WRITE_IR)
        {
            var_insert(code->u.sinop.op);
        }
        else if(code_kind == CALL_IR)
        {
            var_insert(code->u.assign.left);
        }
        else if(code_kind == PARAM_IR)
        {
            //TODO
            //var_insert(code->u.sinop.op);
        }
        func_head = func_head->next;
    }
    fp_offset+=4;
}

void instruction_select(FILE *f)
{
    InterCodes cur = code_root;
    while (cur != NULL)
    {
        InterCode code = cur->code;
        if (code->kind == ASSIGN_IR)
        {
            print_assign(f, code);
        }
        else if (code->kind == ADD_IR)
        {
            print_add(f, code);
        }
        else if (code->kind == SUB_IR)
        {
            print_sub(f, code);
        }
        else if (code->kind == MUL_IR)
        {
            print_mul(f, code);
        }
        else if (code->kind == DIV_IR)
        {
            print_div(f, code);
        }
        else if (code->kind == LABEL_IR)
        {
            
            operand_print(f, code->u.sinop.op);
            fputs(":\n",f);
        }
        else if (code->kind == FUNCTION_IR)
        {
            if(VarMan_head == NULL)
                free_space();
            operand_print(f, code->u.sinop.op);
            fputs(":\n",f);
            fprintf(f, "    addi    $sp, $sp, -4\n");
            fprintf(f, "    sw      $fp, 0($sp)\n");
            fprintf(f, "    move    $fp, $sp\n");

            fp_offset = -4;
            InterCodes func_head = cur->next;
            alloc_space(func_head);

            fprintf(f, "    addi    $sp, $sp, %d\n", fp_offset);
        }
        else if (code->kind == GOTO_IR)
        {
            fputs("\tj\t",f);
            operand_print(f, code->u.sinop.op);
            fputs("\n",f);
        }
        else if (code->kind == IF_IR)
        {
            print_if(f, code);
        }
        else if (code->kind == RETURN_IR)
        {
            print_ret(f, code);
        }
        else if (code->kind == DEC_IR)
        {
            print_dec(f, code);
        }
        else if (code->kind == ARG_IR)
        {
        }
        else if (code->kind == CALL_IR)
        {
        }
        else if (code->kind == PARAM_IR)
        {
        }
        else if (code->kind == READ_IR)
        {
            print_read(f, code);
        }
        else if (code->kind == WRITE_IR)
        {
            print_write(f, code);
        }
        cur = cur->next;
    }
}

void print_assign(FILE* f, InterCode code)
{
    int left_op_kind = code->u.assign.left->kind;
    int right_op_kind = code->u.assign.right->kind;
    VarManager left_var = var_find(code->u.assign.left->u.char_value);
    if(right_op_kind == CONSTANT_OP)
    {
        fprintf(f, "    li  $t0, %d\n", code->u.assign.right->u.int_value);
        fprintf(f, "    sw  $t0, %d($fp)\n",left_var->u.offset);
    }
    else if(right_op_kind == VARIABLE_OP)
    {
        VarManager right_var = var_find(code->u.assign.right->u.char_value);
        if(left_op_kind == VARIABLE_OP)
        {
            //x := y
            fprintf(f, "    lw  $t0, %d($fp)\n", right_var->u.offset);
            fprintf(f, "    sw  $t0, %d($fp)\n", right_var->u.offset);
        }
        else
        {
            //*x := y
            fprintf(f, "    lw  $t0, %d($fp)\n", right_var->u.offset);
            fprintf(f, "    lw  $t1, %d($fp)\n", left_var->u.offset);
            fprintf(f, "    sw  $t0, 0($t1)\n");
        }
    }
    else if(right_op_kind == ADDRTOMEM_OP)
    {
        //x := *y
        VarManager right_var = var_find(code->u.assign.right->u.char_value);
        fprintf(f, "    lw  $t0, %d($fp)\n", right_var->u.offset);
        fprintf(f, "    lw  $t1, 0($t0)\n");
        fprintf(f, "    sw  $t1, %d($fp)\n", left_var->u.offset);
    }
    else if(right_op_kind == MEMTOADDR_OP)
    {
        //x := &y
        VarManager right_var = var_find(code->u.assign.right->u.char_value);
        fprintf(f, "    addi    $t0, $fp, %d\n", right_var->u.offset);
        fprintf(f, "    sw  $t0, %d($fp)\n", left_var->u.offset);
    }

}

void print_add(FILE *f, InterCode code)
{

}

void print_sub(FILE *f, InterCode code)
{

}
void print_mul(FILE *f, InterCode code)
{

}
void print_div(FILE *f, InterCode code)
{
}
void print_if(FILE *f, InterCode code)
{

}
void print_ret(FILE *f, InterCode code)
{

}
void print_dec(FILE *f, InterCode code)
{

}
void print_read(FILE *f, InterCode code)
{

}
void print_write(FILE *f, InterCode code)
{

}