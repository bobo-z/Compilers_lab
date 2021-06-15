#ifndef MIPS_H_
#define MIPS_H_
#include "common.h"
typedef struct VarManager_ *VarManager;
struct VarManager_
{
    Operand var;
    union
    {
        int offset;
        int reg_num;
    } u;
    VarManager next;
};



void mips(char *filename);
void init_print(FILE *f);
void instruction_select(FILE *f);
void alloc_space(InterCodes func_head);
void print_assign(FILE *f, InterCode code);
void print_add(FILE *f, InterCode code);
void print_sub(FILE *f, InterCode code);
void print_mul(FILE *f, InterCode code);
void print_div(FILE *f, InterCode code);
void print_if(FILE *f, InterCode code);
void print_ret(FILE *f, InterCode code);
void print_dec(FILE *f, InterCode code);
void print_read(FILE *f, InterCode code);
void print_write(FILE *f, InterCode code);

#endif