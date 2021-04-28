%locations
%define parse.error verbose

%{
    #include "common.h"
    #define YYSTYPE Node*

    

    extern int yylex(void);
    void yyerror(const char *msg);

    Node* root = NULL;
    int Error = 0;
%}

/* declared types */

/* declared tokens */
%token INT
%token FLOAT
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

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT NEG
%left DOT LB

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


/* declared non-terminals */

%%
/* High-level Definitions */
Program: ExtDefList {
        
        $$=root=InitNode("Program", @$.first_line,SYNTAX_UNIT);
        //printf("in program\n");
        AddChild($$, 1, $1);
    }
    ;
ExtDefList: /* empty */{
        $$ = NULL;
    }
    | ExtDef ExtDefList{
        $$ = InitNode("ExtDefList",@$.first_line, SYNTAX_UNIT);
        AddChild($$,2,$1,$2);
    }
    ;
ExtDef: Specifier ExtDecList SEMI{
        $$ = InitNode("ExtDef", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | Specifier SEMI{
        $$ = InitNode("ExtDef",@$.first_line, SYNTAX_UNIT);
        AddChild($$,2,$1,$2);
    }
    | Specifier FunDec CompSt{
        $$ = InitNode("ExtDef", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    |Specifier FunDec SEMI{
        yyerror("Inconsistent declaration of function");
    }
    | error SEMI{
        yyerrok;
    }
    ;
ExtDecList: VarDec{
        $$ =  InitNode("ExtDecList", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    | VarDec COMMA ExtDecList{
        $$ = InitNode("ExtDecList", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    ;

/* Specifier */
Specifier: TYPE{
        $$ = InitNode("Specifier", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    | StructSpecifier{
        $$ = InitNode("Specifier", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    ;
StructSpecifier: STRUCT OptTag LC DefList RC{
        $$ = InitNode("StructSpecifier", @$.first_line, SYNTAX_UNIT);
        AddChild($$,5,$1,$2,$3,$4,$5);       
    }
    | STRUCT Tag{
        $$ = InitNode("StructSpecifier",@$.first_line, SYNTAX_UNIT);
        AddChild($$,2,$1,$2);
    }
    ;
OptTag: /* empty */{
        $$ = NULL;
    }
    | ID{
        $$ = InitNode("OptTag", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    ;
Tag: ID{
        $$ = InitNode("Tag", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    ;

/* Declarators */
VarDec: ID{
        $$ = InitNode("VarDec", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    | VarDec LB INT RB{
        $$ = InitNode("VarDec", @$.first_line, SYNTAX_UNIT);
        AddChild($$,4,$1,$2,$3,$4);
    }
    | VarDec LB error RB{
        yyerrok;
    }
    ;
FunDec: ID LP VarList RP{
        $$ = InitNode("FunDec", @$.first_line, SYNTAX_UNIT);
        AddChild($$,4,$1,$2,$3,$4);
    }
    | ID LP RP{
        $$ = InitNode("FunDec", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | error RP{
        yyerrok;
    }
    ;
VarList: ParamDec COMMA VarList{
        $$ = InitNode("VarList", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }

    //TODO
    | ParamDec{
        $$ = InitNode("VarList", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    ;
ParamDec: Specifier VarDec{
        $$ = InitNode("ParamDec",@$.first_line, SYNTAX_UNIT);
        AddChild($$,2,$1,$2);
    }
    ;

/* Statement */
CompSt: LC DefList StmtList RC{
        $$ = InitNode("CompSt", @$.first_line, SYNTAX_UNIT);
        AddChild($$,4,$1,$2,$3,$4);
    }
    | error RC{
        yyerrok;
    }
    ;
StmtList: /* empty */{
        $$ = NULL;
    }
    | Stmt StmtList{
        $$ = InitNode("StmtList",@$.first_line, SYNTAX_UNIT);
        AddChild($$,2,$1,$2);
    }
    ;
Stmt: Exp SEMI{
        $$ = InitNode("Stmt",@$.first_line, SYNTAX_UNIT);
        AddChild($$,2,$1,$2);
    }
    | CompSt{
        $$ = InitNode("Stmt", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    | RETURN Exp SEMI{
        $$ = InitNode("Stmt", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{
        $$ = InitNode("Stmt", @$.first_line, SYNTAX_UNIT);
        AddChild($$,5,$1,$2,$3,$4,$5);
    }
    | IF LP Exp RP Stmt ELSE Stmt{
        $$ = InitNode("Stmt", @$.first_line, SYNTAX_UNIT);
        AddChild($$,7,$1,$2,$3,$4,$5,$6,$7);
    }
    | WHILE LP Exp RP Stmt{
        $$ = InitNode("Stmt", @$.first_line, SYNTAX_UNIT);
        AddChild($$,5,$1,$2,$3,$4,$5);
    }
    | error SEMI{
        yyerrok;
    } 
    ;

/* Local Definitions */
DefList: /* empty */{
    $$ = NULL;
    }
    | Def DefList{
        $$ = InitNode("DefList",@$.first_line, SYNTAX_UNIT);
        AddChild($$,2,$1,$2);
    }
    ;
Def: Specifier DecList SEMI{
        $$ = InitNode("Def", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | Specifier error SEMI{
        yyerrok;
    }
    ;
DecList: Dec{
        $$ = InitNode("DecList", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    | Dec COMMA DecList{
        $$ = InitNode("DecList", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    ;
Dec: VarDec{
        $$ = InitNode("Dec", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    | VarDec ASSIGNOP Exp{
        $$ = InitNode("Dec", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    ;

/* Expressions */
Exp: Exp ASSIGNOP Exp{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | Exp AND Exp{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | Exp OR Exp{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | Exp RELOP Exp{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | Exp PLUS Exp{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | Exp MINUS Exp{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | Exp STAR Exp{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | Exp DIV Exp{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | LP Exp RP{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | MINUS Exp %prec NEG{
        $$ = InitNode("Exp",@$.first_line, SYNTAX_UNIT);
        AddChild($$,2,$1,$2);
    }
    | NOT Exp{
        $$ = InitNode("Exp",@$.first_line, SYNTAX_UNIT);
        AddChild($$,2,$1,$2);
    }
    | ID LP Args RP{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,4,$1,$2,$3,$4);
    }
    | ID LP RP{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | Exp LB Exp RB{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,4,$1,$2,$3,$4);
    }
    | Exp DOT ID{
        $$ = InitNode("Exp", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | ID{
        $$ = InitNode("Exp", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    | INT{
        $$ = InitNode("Exp", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    | FLOAT{
        $$ = InitNode("Exp", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    | Exp LB error RB{
        yyerrok;
        //yyerror("Missing ']'");
    }
    | LP error RP{
        yyerrok;
    }
    | ID LP error RP{
        yyerrok;
    }
    ;
Args: Exp COMMA Args{
        $$ = InitNode("Args", @$.first_line, SYNTAX_UNIT);
        AddChild($$,3,$1,$2,$3);
    }
    | Exp{
        $$ = InitNode("Args", @$.first_line,SYNTAX_UNIT);
        AddChild($$, 1, $1);
    }
    | error COMMA Args{
        yyerrok;
    }
    ;
%%
#include "lex.yy.c"

void yyerror(const char* msg){
    Error = 1;
    fprintf(stderr, "Error type B at Line %d: %s.\n", yylloc.first_line, msg);
}