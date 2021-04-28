#ifndef TYPE_H_
#define TYPE_H_
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
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

#endif