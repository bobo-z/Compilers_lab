#ifndef INTERCODES_H_
#define INTERCODES_H_
typedef struct InterCodes_* InterCodes;
#include "type.h"

struct InterCodes_
{
    InterCode code;
    InterCodes prev, next;
};


Operand new_tmp();
Operand new_int_op(int kind, int val);//const,size,address
Operand new_char_op(int kind, char* name);
Operand new_label();
InterCode new_code(int num, int kind, ...);
InterCodes code_insert(InterCode code);
InterCodes code_print(char* filename);
void operand_print(FILE* f, Operand op);

#endif