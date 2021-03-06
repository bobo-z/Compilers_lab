digit [0-9]
letter [_a-zA-Z]
relop >|<|>=|<=|==|!=
blank [ \t\n\r]
%%
int|float {printf("TYPE\n");}
struct {printf("STRUCT\n");}
return {printf("RETURN\n");}
if {printf("IF\n");}
else {printf("ELSE\n");}
while {printf("WHILE\n");}
[1-9]{digit}*|0 { printf("Interger value: %d\n",atoi(yytext));}
([1-9]{digit}*|0)\.{digit}+  { printf("Float value: %f\n",atof(yytext));}
{letter}({letter}|{digit}){0,30} {printf("ID: %s\n",yytext);}
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

"!" {printf("NOT\n");}

"(" {printf("LP\n");}
")" {printf("RP\n");}
"[" {printf("LB\n");}
"]" {printf("RB\n");}
"{" {printf("LC\n");}
"}" {printf("RC\n");}

{blank} {}

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