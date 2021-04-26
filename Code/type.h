#ifndef TYPE_H_
#define TYPE_H_
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
#define INT 1
#define FLOAT 2
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
    char name[NAME_LEN];
    Type type;
    FieldList tail;
};

#endif