#ifndef TYPE_H_
#define TYPE_H_

//lab2 type
typedef struct Type_ *Type;
typedef struct FieldList_ *FieldList;
typedef struct ArgList_ *ArgList;

#define BASIC_INT 1
#define BASIC_FLOAT 2
struct Type_
{
    enum
    {
        BASIC,
        ARRAY,
        STRUCTURE,
        FUNCTION,
        STR_DEF
    } kind;
    union
    {
        int basic;
        struct
        {
            Type elem;
            int size;
        } array;
        FieldList structure;
        struct
        {
            Type ret;
            FieldList param;
        } function;
    } u;
};

struct FieldList_
{
    char name[NAME_LEN];
    Type type;
    FieldList tail;
};

//lab3 type
typedef struct Operand_ *Operand;
typedef struct InterCode_ *InterCode;
struct Operand_
{
    enum
    {
        VARIABLE_OP,
        CONSTANT_OP,
        FUNCTION_OP,
        ADDRESS_OP,
        MEMORY_OP,
        LABEL_OP,
        SIZE_OP,
        RELOP_OP
    } kind;
    union
    {
        int int_value;
        char char_value[NAME_LEN];
    } u;
};
/*
 * ASSIGN:
 *  x := y
 *  x := *y
 *  x := &y
 * *x := y
 *  use struct assign
*/
struct InterCode_
{
    enum
    {
        ASSIGN_IR,
        ADD_IR,
        SUB_IR,
        MUL_IR,
        DIV_IR,
        LABEL_IR,
        FUNCTION_IR,
        GOTO_IR,
        IF_IR,
        RETURN_IR,
        DEC_IR,
        ARG_IR,
        CALL_IR,
        PARAM_IR,
        READ_IR,
        WRITE_IR
    } kind;
    union
    {
        struct
        {
            Operand right, left;
        } assign;
        struct
        {
            Operand result, op1, op2;
        } binop;
        struct
        {
            Operand op;
        } sinop;
        struct
        {
            Operand relop, op1, op2, label;
        } if_code;
    } u;
};

struct ArgList_
{
    Operand op;
    ArgList next;
};

#endif