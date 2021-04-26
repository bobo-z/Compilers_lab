#ifndef SEMANTIC_H_
#define SEMANTIC_H_
#include "common.h"

void semantic(Node* root);
void ExtDefList(Node* p);
void ExtDef(Node* p);
Type Specifer(Node* p);
void ExtDecList(Node* p, Type t);

#endif