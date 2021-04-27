#include "common.h"
//extern FILE* yyin;
extern Node* root;
extern int Error;
extern int yydebug;
extern int yyparse (void);
extern void yyrestart (FILE *input_file  );
int main(int argc, char **argv)
{
    if (argc < 1)
        return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    //yydebug = 1;
    yyparse();
    if(!Error)
    {
        //PrintTree(root,0);
        semantic(root);
    }
    return 0;
    /*lex test
    if (argc > 1)
    {
        if (!(yyin = fopen(argv[1], "r")))
        {
            perror(argv[1]);
            return 1;
        }
    }
    yylex();
    return 0;
    */
}