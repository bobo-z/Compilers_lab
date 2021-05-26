#ifndef TYPE_H_
#define TYPE_H_

//lab2 type
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;
#define BASIC_INT 1
#define BASIC_FLOAT 2
struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE, FUNCTION , STR_DEF} kind;
    union
    {
        int basic;
        struct { Type elem; int size; } array;
        FieldList structure;
        struct { Type ret; FieldList param; } function;
    } u;    
};

struct FieldList_
{
    char name[NAME_LEN];
    Type type;
    FieldList tail;
};

//lab3 type
struct Operand_ 
{
    enum{ VARIABLE, CONSTANT, ADDRESS } kind;
    union
    {
        int var_no;
        int value;
    } u;
};

struct InterCode_
{
    enum { ASSIGN, ADD, SUB, MUL } kind;
    union
    {
        struct { Operand right, left; } assign;
        struct { Operand result, op1, op2; } binop;
    } u;
};


#endif