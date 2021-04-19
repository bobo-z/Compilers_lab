#ifndef TYPE_H_
#define TYPE_H_
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE } kind;
    union
    {
        int basic;
        struct { Type elem; int size; } array;
        FieldList structure;
    } u;    
};

struct FieldList_
{
    char* name;
    Type type;
    FieldList tail;
};

#endif