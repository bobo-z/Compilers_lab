#include "MIPS.h"
extern InterCodes code_root;
int avail_reg = 18;
int fp_offset = -4;
int arg_offset = 0;
int param_offset= 8;

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

VarManager var_find(char *name)
{
    VarManager cur = VarMan_head;
    while (cur != NULL)
    {
        Operand op = cur->var;
        if (0 == strcmp(name, op->u.char_value))
            return cur;
        cur = cur->next;
    }
    return NULL;
}

void var_insert(Operand op)
{
    if (var_find(op->u.char_value) != NULL)
        return;
    if (op->kind != VARIABLE_OP)
        return;
    VarManager vm = (VarManager)malloc(sizeof(struct VarManager_));
    vm->var = op;
    vm->offset = fp_offset;
    fp_offset -= 4;
    vm->next = NULL;
    if (VarMan_head == NULL)
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
    while (VarMan_head != NULL)
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
    while (func_head != NULL && func_head->code->kind != FUNCTION_IR)
    {
        code = func_head->code;
        code_kind = func_head->code->kind;
        if (code_kind == ASSIGN_IR)
        {
            var_insert(code->u.assign.left);
            var_insert(code->u.assign.right);
        }
        else if (code_kind == ADD_IR || code_kind == SUB_IR || code_kind == MUL_IR || code_kind == DIV_IR)
        {
            var_insert(code->u.binop.result);
            var_insert(code->u.binop.op1);
            var_insert(code->u.binop.op2);
        }
        else if (code_kind == IF_IR)
        {
            var_insert(code->u.if_code.op1);
            var_insert(code->u.if_code.op2);
        }
        else if (code_kind == DEC_IR)
        {
            fp_offset += 4;
            fp_offset -= code->u.assign.right->u.int_value;
            var_insert(code->u.assign.left);
            
        }
        else if (code_kind == READ_IR || code_kind == WRITE_IR)
        {
            var_insert(code->u.sinop.op);
        }
        else if (code_kind == CALL_IR)
        {
            var_insert(code->u.assign.left);
        }
        else if (code_kind == PARAM_IR)
        {
            //TODO
            var_insert(code->u.sinop.op);
            fp_offset += 4;
        }
        func_head = func_head->next;
    }
    fp_offset += 4;
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
            fputs(":\n", f);
        }
        else if (code->kind == FUNCTION_IR)
        {
            param_offset = 8;
            if (VarMan_head == NULL)
                free_space();
            operand_print(f, code->u.sinop.op);
            fputs(":\n", f);
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
            fputs("\tj\t", f);
            operand_print(f, code->u.sinop.op);
            fputs("\n", f);
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
            VarManager op = var_find(code->u.sinop.op->u.char_value);
            fprintf(f, "    lw  $t0, %d($fp)\n", op->offset);
            fprintf(f, "    addi    $sp, $sp, -4\n");
            arg_offset -= 4;
            fprintf(f, "    sw  $t0, 0($sp)\n");
        }
        else if (code->kind == CALL_IR)
        {
            fprintf(f, "  addi $sp, $sp, -4\n");
            fprintf(f, "  sw $ra, 0($sp)\n");
            fprintf(f, "  jal %s\n", code->u.assign.right->u.char_value);
            fprintf(f, "  lw $ra, 0($sp)\n");
            fprintf(f, "  addi $sp, $sp, 4\n");
            if (arg_offset != 0)
            {
                fprintf(f, "  addi $sp, $sp, %d\n", -arg_offset);
                arg_offset = 0;
            }
            VarManager ret_val = var_find(code->u.assign.left->u.char_value);
            fprintf(f, "  sw $v0, %d($fp)\n", ret_val->offset);
        }
        else if (code->kind == PARAM_IR)
        {
            VarManager op = var_find(code->u.sinop.op->u.char_value);
            op->offset = param_offset;
            param_offset += 4;
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

void print_assign(FILE *f, InterCode code)
{
    int left_op_kind = code->u.assign.left->kind;
    int right_op_kind = code->u.assign.right->kind;
    VarManager left_var = var_find(code->u.assign.left->u.char_value);
    if (right_op_kind == CONSTANT_OP)
    {
        fprintf(f, "    li  $t0, %d\n", code->u.assign.right->u.int_value);
        fprintf(f, "    sw  $t0, %d($fp)\n", left_var->offset);
    }
    else if (right_op_kind == VARIABLE_OP)
    {
        VarManager right_var = var_find(code->u.assign.right->u.char_value);
        if (left_op_kind == VARIABLE_OP)
        {
            //x := y
            fprintf(f, "    lw  $t0, %d($fp)\n", right_var->offset);
            fprintf(f, "    sw  $t0, %d($fp)\n", left_var->offset);
        }
        else
        {
            //*x := y
            fprintf(f, "    lw  $t0, %d($fp)\n", right_var->offset);
            fprintf(f, "    lw  $t1, %d($fp)\n", left_var->offset);
            fprintf(f, "    sw  $t0, 0($t1)\n");
        }
    }
    else if (right_op_kind == ADDRTOMEM_OP)
    {
        //x := *y
        VarManager right_var = var_find(code->u.assign.right->u.char_value);
        fprintf(f, "    lw  $t0, %d($fp)\n", right_var->offset);
        fprintf(f, "    lw  $t1, 0($t0)\n");
        fprintf(f, "    sw  $t1, %d($fp)\n", left_var->offset);
    }
    else if (right_op_kind == MEMTOADDR_OP)
    {
        //x := &y
        VarManager right_var = var_find(code->u.assign.right->u.char_value);
        fprintf(f, "    addi    $t0, $fp, %d\n", right_var->offset);
        fprintf(f, "    sw  $t0, %d($fp)\n", left_var->offset);
    }
}

void print_add(FILE *f, InterCode code)
{
    // x := y + z
    // x := &y + z
    VarManager op1 = var_find(code->u.binop.op1->u.char_value);
    if (code->u.binop.op1->kind == VARIABLE_OP)
    {
        fprintf(f, "    lw  $t1, %d($fp)\n", op1->offset);
    }
    else
    {
        assert(code->u.binop.op1->kind == MEMTOADDR_OP);
        fprintf(f, "    addi    $t1, $fp, %d\n", op1->offset);
    }
    VarManager res = var_find(code->u.binop.result->u.char_value);
    VarManager op2 = var_find(code->u.binop.op2->u.char_value);
    fprintf(f, "    lw  $t2, %d($fp)\n", op2->offset);
    fprintf(f, "    add $t0, $t1, $t2\n");
    fprintf(f, "    sw  $t0, %d($fp)\n", res->offset);
}

void print_sub(FILE *f, InterCode code)
{
    VarManager res = var_find(code->u.binop.result->u.char_value);
    VarManager op2 = var_find(code->u.binop.op2->u.char_value);
    fprintf(f, "    lw  $t2, %d($fp)\n", op2->offset);

    if (code->u.binop.op1->kind == VARIABLE_OP)
    {
        VarManager op1 = var_find(code->u.binop.op1->u.char_value);
        fprintf(f, "    lw  $t1, %d($fp)\n", op1->offset);
        fprintf(f, "    sub $t0, $t1, $t2\n");
    }
    else
    {
        assert(code->u.binop.op1->kind == CONSTANT_OP);
        fprintf(f, "    sub $t0, $0, $t2\n");
    }

    fprintf(f, "    sw  $t0, %d($fp)\n", res->offset);
}
void print_mul(FILE *f, InterCode code)
{
    //x := y * z
    //x := y * #4

    if (code->u.binop.op2->kind == VARIABLE_OP)
    {
        VarManager op2 = var_find(code->u.binop.op2->u.char_value);
        fprintf(f, "    lw  $t2, %d($fp)\n", op2->offset);
    }
    else
    {
        assert(code->u.binop.op2->kind == CONSTANT_OP);
        fprintf(f, "    li  $t2, 4\n");
    }
    VarManager res = var_find(code->u.binop.result->u.char_value);
    VarManager op1 = var_find(code->u.binop.op1->u.char_value);
    fprintf(f, "    lw  $t1, %d($fp)\n", op1->offset);
    fprintf(f, "    mul $t0, $t1, $t2\n");
    fprintf(f, "    sw  $t0, %d($fp)\n", res->offset);
}
void print_div(FILE *f, InterCode code)
{
    VarManager res = var_find(code->u.binop.result->u.char_value);
    VarManager op1 = var_find(code->u.binop.op1->u.char_value);
    VarManager op2 = var_find(code->u.binop.op2->u.char_value);
    fprintf(f, "    lw  $t1, %d($fp)\n", op1->offset);
    fprintf(f, "    lw  $t2, %d($fp)\n", op2->offset);
    fprintf(f, "    div $t1, $t2\n");
    fprintf(f, "    mflo    $t0\n");
    fprintf(f, "    sw  $t0, %d($fp)\n", res->offset);
}
void print_if(FILE *f, InterCode code)
{
    VarManager op1 = var_find(code->u.if_code.op1->u.char_value);
    fprintf(f, "    lw  $t1, %d($fp)\n", op1->offset);
    if (code->u.if_code.op2->kind == VARIABLE_OP)
    {
        VarManager op2 = var_find(code->u.if_code.op2->u.char_value);
        fprintf(f, "    lw  $t2, %d($fp)\n", op2->offset);
    }

    if (0 == strcmp(code->u.if_code.relop->u.char_value, "=="))
    {
        fprintf(f, "    beq $t1, $t2, ");
    }
    else if (0 == strcmp(code->u.if_code.relop->u.char_value, "!="))
    {
        if (code->u.if_code.op2->kind == CONSTANT_OP)
            fprintf(f, "    bne $t1, $0, ");
        else
            fprintf(f, "    bne $t1, $t2, ");
    }
    else if (0 == strcmp(code->u.if_code.relop->u.char_value, ">"))
    {
        fprintf(f, "    bgt $t1, $t2, ");
    }
    else if (0 == strcmp(code->u.if_code.relop->u.char_value, "<"))
    {
        fprintf(f, "    blt $t1, $t2, ");
    }
    else if (0 == strcmp(code->u.if_code.relop->u.char_value, ">="))
    {
        fprintf(f, "    bge $t1, $t2, ");
    }
    else if (0 == strcmp(code->u.if_code.relop->u.char_value, "<="))
    {
        fprintf(f, "    ble $t1, $t2, ");
    }
    operand_print(f, code->u.if_code.label);
    fputs("\n", f);
}
void print_ret(FILE *f, InterCode code)
{
    VarManager ret = var_find(code->u.sinop.op->u.char_value);

    fprintf(f, "    lw  $t0, %d($fp)\n", ret->offset);
    fprintf(f, "    move    $v0, $t0\n");
    fprintf(f, "  move $sp, $fp\n");
    fprintf(f, "  lw $fp, 0($sp)\n");
    fprintf(f, "  addi $sp, $sp, 4\n");
    fprintf(f, "    jr  $ra\n\n");
}
void print_dec(FILE *f, InterCode code)
{
}
void print_read(FILE *f, InterCode code)
{
    VarManager ret = var_find(code->u.sinop.op->u.char_value);
    fprintf(f, "    addi    $sp, $sp, -4\n");
    fprintf(f, "    sw  $ra, 0($sp)\n");
    fprintf(f, "    jal read\n");
    fprintf(f, "    lw, $ra, 0($sp)\n");
    fprintf(f, "    addi    $sp, $sp, 4\n");

    fprintf(f, "    sw  $v0, %d($fp)\n", ret->offset);
}
void print_write(FILE *f, InterCode code)
{
    VarManager arg = var_find(code->u.sinop.op->u.char_value);
    fprintf(f, "    lw  $a0, %d($fp)\n", arg->offset);

    fprintf(f, "    addi    $sp, $sp, -4\n");
    fprintf(f, "    sw  $ra, 0($sp)\n");
    fprintf(f, "    jal write\n");
    fprintf(f, "    lw, $ra, 0($sp)\n");
    fprintf(f, "    addi    $sp, $sp, 4\n");
}