#include "common.h"

InterCodes code_root = NULL, code_tail = NULL;
int tmp_count = 0, label_count = 0;
Operand new_tmp()
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = VARIABLE_OP;
    op->u.int_value = tmp_count;
    tmp_count++;
    return op;
}

Operand new_const(int kind, int val)
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = kind;
    op->u.int_value = val;
    return op;
}

Operand new_label()
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = LABEL_OP;
    op->u.int_value = label_count;
    label_count++;
    return op;
}

InterCode new_code(int num, int kind, ...)
{
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    va_list vl;
    va_start(vl, kind);
    code->kind = kind;
    switch (num)
    {
    case 1:
        //sinop
        code->u.sinop.op = va_arg(vl,Operand);
        break;
    case 2:
        //assign
        code->u.assign.left = va_arg(vl, Operand);
        code->u.assign.right = va_arg(vl, Operand);
        break;
    case 3:
        //binop
        code->u.binop.result = va_arg(vl, Operand);
        code->u.binop.op1 = va_arg(vl, Operand);
        code->u.binop.op2 = va_arg(vl, Operand);
        break;
    case 4:
        code->u.if_code.op1 = va_arg(vl, Operand);
        code->u.if_code.relop = va_arg(vl, Operand);
        code->u.if_code.op2 = va_arg(vl, Operand);
        code->u.if_code.label = va_arg(vl, Operand);
        break;
    default:
        break;
    }
    va_end(vl);
    return code;
}

InterCodes code_insert(InterCode code)
{
    InterCodes i = (InterCodes)malloc(sizeof(struct InterCodes_));
    i->code = code;
    i->prev = NULL;
    i->next = NULL;
    if(code_root == NULL)
    {
        code_root = i;
        code_tail = i;
    }
    else
    {
        code_tail->next = i;
        i->prev = code_tail;
        code_tail = i;
    }
    return i;
}

InterCodes code_print(char* filename)
{
    FILE* f = fopen(filename, "w");
    InterCodes cur = code_root;
    InterCode code = NULL;
    while(cur!=NULL)
    {
        code = cur->code;
        switch (code->kind)
        {
        case ASSIGN_IR:
            operand_print(f, code->u.assign.left);
            fputs(" := ",f);
            operand_print(f, code->u.assign.right);
            break;
        case ADD_IR:case SUB_IR:case MUL_IR:case DIV_IR:
            operand_print(f, code->u.binop.result);
            fputs(" := ",f);
            operand_print(f, code->u.binop.op1);
            switch (code->kind)
            {
            case ADD_IR:
                fputs(" + ",f);
                break;
            case SUB_IR:
                fputs(" - ",f);
                break;
            case MUL_IR:
                fputs(" * ",f);
                break;
            case DIV_IR:
                fputs(" / ",f);
                break;
            default:
                break;
            }
            operand_print(f, code->u.binop.op2);
            break;
        case LABEL_IR:
            fputs("LABEL ",f);
            operand_print(f, code->u.sinop.op);
            fputs(" :",f);
            break;
        case FUNCTION_IR:
            fputs("FUNCTION ",f);
            operand_print(f,code->u.sinop.op);
            fputs(" :",f);
            break;
        case GOTO_IR:
            fputs("GOTO ",f);
            operand_print(f,code->u.sinop.op);
            break;
        case RETURN_IR:
            fputs("RETURN ",f);
            operand_print(f,code->u.sinop.op);
            break;
        case ARG_IR:
            fputs("ARG ",f);
            operand_print(f,code->u.sinop.op);
            break;
        case PARAM_IR:
            fputs("PARAM ",f);
            operand_print(f,code->u.sinop.op);
            break;
        case READ_IR:
            fputs("READ ",f);
            operand_print(f,code->u.sinop.op);
            break;
        case WRITE_IR:
            fputs("WRITE ",f);
            operand_print(f,code->u.sinop.op);
            break;
        case IF_IR:
            fputs("IF ",f);
            operand_print(f,code->u.if_code.op1);
            fputs(" ",f);
            operand_print(f,code->u.if_code.relop);
            fputs(" ",f);
            operand_print(f,code->u.if_code.op2);
            fputs(" GOTO ",f);
            operand_print(f,code->u.if_code.label);
            break;
        case DEC_IR:
            fputs("DEC ",f);
            operand_print(f, code->u.assign.left);
            fputs(" ",f);
            operand_print(f,code->u.assign.right);
            break;
        case CALL_IR:
            operand_print(f,code->u.assign.left);
            fputs(" := CALL ",f);
            operand_print(f, code->u.assign.right);
            break;
        default:
            break;
        }
        fputs("\n",f);
        cur=cur->next;
    }
    return NULL;
}

void operand_print(FILE* f, Operand op)
{
    switch(op->kind)
    {
        case VARIABLE_OP:
            fprintf(f,"t%d",op->u.int_value);
            break;
        case CONSTANT_OP:
            fprintf(f,"#%d", op->u.int_value);
            break;
        case ADDRESS_OP:
            fprintf(f, "&t%d",op->u.int_value);
            break;
        case MEMORY_OP:
            fprintf(f,"*t%d",op->u.int_value);
            break;
        case LABEL_OP:
            fprintf(f,"label%d",op->u.int_value);
            break;
        case SIZE_OP:
            fprintf(f,"%d",op->u.int_value);
            break;
        case RELOP_OP:
            fputs(op->u.char_value, f);
            break;
        default:
            assert(0);
            break;
    }
}