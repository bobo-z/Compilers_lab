#ifndef IR_H_
#define IR_H_
#include"common.h"

void ir(Node* p);
void translate_ExtDefList(Node* p);
void translate_ExtDef(Node* p);
void translate_FunDec(Node* p);
void translate_Compst(Node* p);
void translate_DefList(Node* p);
void translate_StmtList(Node* p);
void translate_Stmt(Node* p);
void translate_Exp(Node* p, Operand place);
void translate_Cond(Node* p, Operand label_true, Operand label_false);
ArgList translate_Args(Node* p, ArgList head);
int array_size(Type t);

#endif