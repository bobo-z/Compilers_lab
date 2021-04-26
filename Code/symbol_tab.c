#include "common.h"

FieldList symbol_tab[TAB_SIZE];

FieldList init()
{
    int i;
    for(i=0;i<TAB_SIZE;i++)
        symbol_tab[i] = NULL;
}
unsigned int hash_bkdr(char* name)
{
    unsigned int hash = 0;
    unsigned int seed = 131;
    while(*name)
    {
        hash = hash * seed + (*name++);
    }
    return hash%32767;
}

FieldList find(char* name)
{
    unsigned int key = hash_bkdr(name)%TAB_SIZE;
    FieldList p = symbol_tab[key];
    while(p!=NULL)
    {
        if(strcmp(p->name, name)==0)
        {
            return p;
        }
        p = p->tail;
    }
    return NULL;
}

FieldList insert(char* name, Type type)
{
    unsigned int key = hash_bkdr(name)%TAB_SIZE;
    FieldList p = (FieldList)malloc(sizeof(struct FieldList_));
    p->name = name;
    p->type =type;
    p->tail = NULL;
    if(symbol_tab[key]==NULL)
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
