#ifndef SEMANTIC_H_
#define SEMANTIC_H_
#include "common.h"

void semantic(Node* p);
void ExtDefList(Node* p);
void ExtDef(Node* p);
Type Specifer(Node* p);
void ExtDecList(Node* p, Type t);
FieldList VarDec(Node *p, Type t, int struct_def);
void FunDec(Node* p, Type t);
void CompSt(Node* p, Type ret);
void Stmt(Node* p, Type ret);
FieldList DefList(Node *p, int struct_def);
Type Exp(Node *p);
int Type_Check(Type t1, Type t2);
int Struct_Def_exist(FieldList f, char* name);

#endif