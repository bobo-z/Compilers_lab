#include "common.h"
//extern FILE* yyin;
extern Node* root;
extern int Error;
extern int yydebug;
extern int yyparse (void);
extern void yyrestart (FILE *input_file  );

//#define TEST

void test()
{
    #ifdef TEST
    Operand main = new_tmp();
    code_insert(new_code(1,FUNCTION_IR,main));
    Operand t1 = new_tmp();
    code_insert(new_code(1,READ_IR,t1));
    Operand v1 = new_tmp();
    code_insert(new_code(2, ASSIGN_IR, v1, t1));
    Operand t2 = new_tmp();
    Operand zero = new_const(CONSTANT_OP,0);
    code_insert(new_code(2, ASSIGN_IR, t2, zero));
    Operand xiaoyu = new_tmp();
    xiaoyu->kind = RELOP_OP;
    strncpy(xiaoyu->u.char_value,"<",NAME_LEN);
    Operand dayu = new_tmp();
    dayu->kind = RELOP_OP;
    strncpy(dayu->u.char_value,">",NAME_LEN);
    Operand label1 = new_label();
    Operand label2 = new_label();
    code_insert(new_code(4, IF_IR, v1, dayu, t2, label1));
    code_insert(new_code(4, IF_IR, v1, xiaoyu, t2, label2));
    code_insert(new_code(1, WRITE_IR, t2));
    Operand label3 = new_label();
    code_insert(new_code(1, GOTO_IR, label3));
    code_insert(new_code(1, LABEL_IR, label1));
    Operand t3 = new_tmp();
    code_insert(new_code(2, ASSIGN_IR, t3, new_const(CONSTANT_OP, 1)));
    code_insert(new_code(1, WRITE_IR, t3));
    code_insert(new_code(1, GOTO_IR, label3));
    code_insert(new_code(1, LABEL_IR, label2));
    Operand t6 = new_tmp();
    code_insert(new_code(2, ASSIGN_IR, t6, new_const(CONSTANT_OP, -1)));
    code_insert(new_code(1, WRITE_IR, t6));
    code_insert(new_code(1, LABEL_IR, label3));
    code_insert(new_code(1, RETURN_IR, t2));
    #endif

}


int main(int argc, char **argv)
{
    #ifdef TEST
    test();
    #else
    if (argc < 1)
        return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    if(!Error)
    {
        //PrintTree(root,0);
        semantic(root);
        ir(root);
        code_print("out.ir");
        mips(argv[2]);
    }
    //fprintf(stderr, "s\n");
    //code_print(argv[2]);
    fclose(f);
    #endif
    return 0;
}