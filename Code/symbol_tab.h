#ifndef SYMBOL_TAB_H_
#define SYMBOL_TAB_H_

#include "type.h"

#define NAME_LEN 30
#define TAB_SIZE 16384

void init();
FieldList insert(char* name, Type type);
FieldList find(char* name, int function_find);
unsigned int hash_pjw(char* name);


#endif