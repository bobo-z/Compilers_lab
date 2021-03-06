%option yylineno

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
{type} {printf("TYPE\n");}
struct {printf("STRUCT\n");}
return {printf("RETURN\n");}
if {printf("IF\n");}
else {printf("ELSE\n");}
while {printf("WHILE\n");}
{int} { printf("Interger value: %d\n",atoi(yytext));}
{float}  { printf("Float value: %f\n",atof(yytext));}
{id} {printf("ID: %s\n",yytext);}
"/*"[^\*]*[\*]+([^\*\/][^\*]*[\*]+)*"/" {
    printf("BLOCKd COMMENT\n");
}
"//".* {
    printf("LINE COMMENT\n");
}
";" {printf("SEMI\n");}
"," {printf("COMMA\n");}
"=" {printf("ASSIGNOP\n");}
">"|"<"|">="|"<="|"=="|"!=" {printf("RELOP\n");}
"+" {printf("PLUS\n");}
"-" {printf("MINUS\n");}
"*" {printf("STAR\n");}
"/" {printf("DIV\n");}
"&&" {printf("AND\n");}
"||" {printf("OR\n");}
"." {printf("DOT\n");}
"!" {printf("NOT\n");}
"(" {printf("LP\n");}
")" {printf("RP\n");}
"[" {printf("LB\n");}
"]" {printf("RB\n");}
"{" {printf("LC\n");}
"}" {printf("RC\n");}

{ws} {}



. {
printf("Error type A at Line %d: Mysterious characters \'%s\'\n",
yylineno, yytext);
}
%%
int main(int argc, char **argv)
{
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
}