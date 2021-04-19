#ifndef SYMBOL_TAB_H_
#define SYMBOL_TAB_H_

#include "type.h"

#define NAME_LEN 30
#define TAB_SIZE 10000

typedef struct Symbol_* Symbol;
struct Symbol_
{
    char name[NAME_LEN];
    Type type;
    Symbol next;
};
Symbol symbol_tab[TAB_SIZE];


Symbol init();
Symbol insert(char* name, Type type);
Symbol find(char* name);
Symbol struct_find(char* name, Type type);
unsigned int hash_bkdr(char* name);


#endif