#ifndef SYMBOL_TAB_H_
#define SYMBOL_TAB_H_

#include "type.h"

#define NAME_LEN 30
#define TAB_SIZE 10000

FieldList init();
FieldList insert(char* name, Type type);
FieldList find(char* name, int function_find);
FieldList struct_find(char* name, Type type);
unsigned int hash_bkdr(char* name);


#endif