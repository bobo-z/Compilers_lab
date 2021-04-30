#ifndef SEMANTIC_H_
#define SEMANTIC_H_
#include "common.h"
//main entry
void semantic(Node* p);

//different not-terminal, different situation
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
int Args(Node *p,FieldList f);

//help function
int Type_Check(Type t1, Type t2);
int Struct_Def_exist(FieldList f, char* name);
int left_hand_check(Node* p);

#endif