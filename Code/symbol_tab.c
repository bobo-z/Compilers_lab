#include "common.h"

FieldList symbol_tab[TAB_SIZE];

void init()
{
    int i;
    for (i = 0; i < TAB_SIZE; i++)
    {
        symbol_tab[i] = NULL;
    }
    
    Type int_type = (Type)malloc(sizeof(struct Type_));
    int_type->kind = BASIC;
    int_type->u.basic = BASIC_INT;
    Type read_func = (Type)malloc(sizeof(struct Type_));
    read_func->kind = FUNCTION;
    read_func->u.function.ret = int_type;
    read_func->u.function.param = NULL;
    Type write_func = (Type)malloc(sizeof(struct Type_));
    write_func->kind = FUNCTION;
    write_func->u.function.ret = int_type;
    FieldList f = (FieldList)malloc(sizeof(struct FieldList_));
    f->tail = NULL;
    f->type = int_type;
    write_func->u.function.param = f;

    insert("read", read_func);
    insert("write", write_func);
}
unsigned int hash_pjw(char *name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff)
            val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

FieldList find(char *name, int function_find)
{
    //function_find
    //1: compare name with function name
    //0: compare name with var or struct
    // printf("in find %s\n",name);
    unsigned int key = hash_pjw(name);
    // printf("%s-%d\n", name, key);
    FieldList p = symbol_tab[key];
    while (p != NULL)
    {
        if (strcmp(p->name, name) == 0)
        {
            if (function_find == 1 && p->type->kind == FUNCTION)
                return p;
            else if (function_find == 0 && p->type->kind != FUNCTION)
                return p;
        }
        p = p->tail;
    }
    //printf("find null\n");
    return NULL;
}

FieldList insert(char *name, Type type)
{

    unsigned int key = hash_pjw(name) % TAB_SIZE;
    FieldList p = (FieldList)malloc(sizeof(struct FieldList_));
    //p->name = name;
    strncpy(p->name, name, NAME_LEN);
    p->type = type;
    p->tail = NULL;
    if (NULL == symbol_tab[key])
    {
        symbol_tab[key] = p;
    }
    else
    {
        p->tail = symbol_tab[key];
        symbol_tab[key] = p;
    }
    return p;
}
