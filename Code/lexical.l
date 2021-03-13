
%option yylineno

%{
    #include "syntax.tab.h"
    #include "common.h"
    
    extern int Error;
    int yycolumn = 1;
    #define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;
%}

digit [0-9]
letter [_a-zA-Z]

relop >|<|>=|<=|==|!=
delim [ \t\r]
ws {delim}+

int [1-9]{digit}*|0
float {int}\.{digit}+
id {letter}({letter}|{digit})*
type int|float


%%
{type} {
    /*printf("TYPE\n");*/
    yylval = InitNode("TYPE",yylineno, CHAR_UNIT);
    strncpy(yylval->char_val, yytext, NAME_LEN);
    //yylval->char_val = yytext;
    return TYPE;
}
struct {
    /*printf("STRUCT\n");*/
    yylval = InitNode("STRUCT",yylineno,OTHER_UNIT);
    return STRUCT;
}
return {
    /*printf("RETURN\n");*/
    yylval = InitNode("RETURN",yylineno,OTHER_UNIT);
    return RETURN;
}
if {
    /*printf("IF\n");*/
    yylval = InitNode("IF",yylineno,OTHER_UNIT);
    return IF;
}
else {
    /*printf("ELSE\n");*/
    yylval = InitNode("ELSE",yylineno,OTHER_UNIT);
    return ELSE;
}
while {
    /*printf("WHILE\n");*/
    yylval = InitNode("WHILE",yylineno,OTHER_UNIT);
    return WHILE;
}
{int} {
    /*printf("Interger value: %d\n",atoi(yytext));*/
    yylval = InitNode("INT",yylineno,INT_UNIT);
    yylval->int_val = atoi(yytext);
    return INT;
}
{float}  {
    /*printf("Float value: %f\n",atof(yytext));*/
    yylval = InitNode("FLOAT",yylineno,FLOAT_UNIT);
    yylval->float_val = atof(yytext);
    return FLOAT;
}
{id} {
    /*printf("ID: %s\n",yytext);*/
    yylval = InitNode("ID",yylineno,CHAR_UNIT);
    strncpy(yylval->char_val, yytext, NAME_LEN);
    return ID;
}
"/*"[^\*]*[\*]+([^\*\/][^\*]*[\*]+)*"/" {
    /*printf("BLOCK COMMENT\n");*/
}
"//".* {
    /*printf("LINE COMMENT\n");*/
}
";" {
    /*printf("SEMI\n");*/
    yylval = InitNode("SEMI",yylineno,OTHER_UNIT);
    return SEMI;
}
"," {
    /*printf("COMMA\n");*/
    yylval = InitNode("COMMA",yylineno,OTHER_UNIT);
    return COMMA;
}
"=" {
    /*printf("ASSIGNOP\n");*/
    yylval = InitNode("ASSIGNOP",yylineno,OTHER_UNIT);
    return ASSIGNOP;
}
{relop} {
    /*printf("RELOP\n");*/
    yylval = InitNode("RELOP",yylineno,OTHER_UNIT);
    return RELOP;
}
"+" {
    /*printf("PLUS\n");*/
    yylval = InitNode("PLUS",yylineno,OTHER_UNIT);
    return PLUS;
}
"-" {
    /*printf("MINUS\n");*/
    yylval = InitNode("MINUS",yylineno,OTHER_UNIT);
    return MINUS;
}
"*" {
    /*printf("STAR\n");*/
    yylval = InitNode("STAR",yylineno,OTHER_UNIT);
    return STAR;
}
"/" {
    /*printf("DIV\n");*/
    yylval = InitNode("DIV",yylineno,OTHER_UNIT);
    return DIV;
}
"&&" {
    /*printf("AND\n");*/
    yylval = InitNode("AND",yylineno,OTHER_UNIT);
    return AND;
}
"||" {
    /*printf("OR\n");*/
    yylval = InitNode("OR",yylineno,OTHER_UNIT);
    return OR;
}
"." {
    /*printf("DOT\n");*/
    yylval = InitNode("DOT",yylineno,OTHER_UNIT);
    return DOT;
}
"!" {
    /*printf("NOT\n");*/
    yylval = InitNode("NOT",yylineno,OTHER_UNIT);
    return NOT;
}
"(" {
    /*printf("LP\n");*/
    yylval = InitNode("LP",yylineno,OTHER_UNIT);
    return LP;
}
")" {
    /*printf("RP\n");*/
    yylval = InitNode("RP",yylineno,OTHER_UNIT);
    return RP;
}
"[" {
    /*printf("LB\n");*/
    yylval = InitNode("LB",yylineno,OTHER_UNIT);
    return LB;
}
"]" {
    /*printf("RB\n");*/
    yylval = InitNode("RB",yylineno,OTHER_UNIT);
    return RB;
}
"{" {
    /*printf("LC\n");*/
    yylval = InitNode("LC",yylineno,OTHER_UNIT);
    return LC;
}
"}" {
    /*printf("RC\n");*/
    yylval = InitNode("RC",yylineno,OTHER_UNIT);
    return RC;
}

\n { yycolumn = 1;}

{ws} {}



. {
    Error = 1;
    fprintf(stderr,"Error type A at Line %d: Mysterious characters \'%s\'\n",
yylineno, yytext);
}

%%

