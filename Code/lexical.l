
%option yylineno

digit [0-9]
letter [_a-zA-Z]

relop >|<|>=|<=|==|!=
delim [ \t\r\n]
ws {delim}+

int [1-9]{digit}*|0
float {int}\.{digit}+
id {letter}({letter}|{digit})*
type int|float


%%
{type} {
    /*printf("TYPE\n");*/
    return TYPE;
}
struct {
    /*printf("STRUCT\n");*/
    return STRUCT;
}
return {
    /*printf("RETURN\n");*/
    return RETURN;
}
if {
    /*printf("IF\n");*/
    return IF;
}
else {
    /*printf("ELSE\n");*/
    return ELSE;
}
while {
    /*printf("WHILE\n");*/
    return WHILE;
}
{int} {
    /*printf("Interger value: %d\n",atoi(yytext));*/
    return INT;
}
{float}  {
    /*printf("Float value: %f\n",atof(yytext));*/
    return FLOAT;
}
{id} {
    /*printf("ID: %s\n",yytext);*/
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
    return SEMI;
}
"," {
    /*printf("COMMA\n");*/
    return COMMA;
}
"=" {
    /*printf("ASSIGNOP\n");*/
    return ASSIGNOP;
}
{relop} {
    /*printf("RELOP\n");*/
    return RELOP;
}
"+" {
    /*printf("PLUS\n");*/
    return PLUS;
}
"-" {
    /*printf("MINUS\n");*/
    return MINUS;
}
"*" {
    /*printf("STAR\n");*/
    return STAR;
}
"/" {
    /*printf("DIV\n");*/
    return DIV;
}
"&&" {
    /*printf("AND\n");*/
    return AND;
}
"||" {
    /*printf("OR\n");*/
    return OR;
}
"." {
    /*printf("DOT\n");*/
    return DOT;
}
"!" {
    /*printf("NOT\n");*/
    return NOT;
}
"(" {
    /*printf("LP\n");*/
    return LP;
}
")" {
    /*printf("RP\n");*/
    return RP;
}
"[" {
    /*printf("LB\n");*/
    return LB;
}
"]" {
    /*printf("RB\n");*/
    return RB;
}
"{" {
    /*printf("LC\n");*/
    return LC;
}
"}" {
    /*printf("RC\n");*/
    return RC;
}

{ws} {}



. {
printf("Error type A at Line %d: Mysterious characters \'%s\'\n",
yylineno, yytext);
}

%%
#include "syntax.tab.h"
