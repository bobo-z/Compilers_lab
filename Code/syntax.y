%{
    #include <stdio.h>
%}

/* declared types */
%union {
    int type_int;
    float type_float;
    double type_double;
}

/* declared tokens */
%token <type_int> INT
%token <type_float> FLOAT
%token ID
%token SEMI COMMA
%token ASSIGNOP RELOP
%token PLUS MINUS STAR DIV
%token AND OR
%token DOT NOT
%token TYPE 
%token LP RP LB RB LC RC
%token STRUCT 
%token RETURN
%token IF ELSE WHILE


/* declared non-terminals */

%%
/* High-level Definitions */
Program: ExtDefList;
ExtDefList: /* empty */
    | ExtDef ExtDefList
    ;
ExtDef: Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    ;
ExtDecList: VarDec
    | VarDec COMMA ExtDecList
    ;

/* Specifier */
Specifier: TYPE
    | StructSpecifier
    ;
StructSpecifier: STRUCT OptTag LC DefList RC
    | STRUCT Tag
    ;
OptTag: /* empty */
    | ID
    ;
Tag: ID
    ;

/* Declarators */
VarDec: ID
    | VarDec LB INT RB
    ;
FunDec: ID LP VarList RP
    | ID LP RP
    ;
VarList: ParamDec COMMA VarList
    | ParamDec
    ;
ParamDec: Specifier VarDec
    ;

/* Statement */
CompSt: LC DefList StmtList RC
    ;
StmtList: /* empty */
    | Stmt StmtList
    ;
Stmt: Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    ;

/* Local Definitions */
DefList: /* empty */
    | Def DefList
    ;
Def: Specifier DecList SEMI
    ;
DecList: Dec
    | Dec COMMA DecList
    ;
Dec: VarDec
    | VarDec ASSIGNOP Exp
    ;

/* Expressions */
Exp: Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp RELOP Exp
    | Exp PLUS Exp
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
    | LP Exp RP
    | MINUS Exp
    | NOT Exp
    | ID LP Args RP
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
    ;
Args: Exp COMMA Args
    | Exp
    ;
%%
#include "lex.yy.c"
yyerror(char* msg){
    fprintf(stderr, "error: %s\n",msg);
}