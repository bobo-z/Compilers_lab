#include "semantic.h"
int annoy_struct_num = 0;
void semantic(Node* p)
{
    if(strcmp(p->name,"Program")==0)
        ExtDefList(p->children[0]);
}

void ExtDefList(Node* p)
{
    if(p==NULL)
    {
        return;
    }
    else
    {
        ExtDef(p->children[0]);
        ExtDefList(p->children[1]);
    }
}

void ExtDef(Node* p)
{
    if(p->child_num == 2)
    {
        //ExtDef: Specifer SEMI
        Specifer(p->children[0]);
    }
    else if(strcmp(p->children[2]->name,"SEMI")==0)
    {
        //ExtDef: Specifier ExtDecList SEMI
        Type t = Specifer(p->children[0]);
        ExtDecList(p->children[1],t);
    }
    else
    {
        //ExtDef: Specifier FunDec CompSt
        Type t = Specifer(p->children[0]);
        FunDec(p->children[1]);
        CompSt(p->children[2]);
    }
}

Type Specifer(Node* p)
{
    
    if(p->children[0]->type == SYNTAX_UNIT)
    {
        //Specifer: StructSpecifier
        
        Node* p_struct = p->children[0];//StructSpecifer
        if(p_struct->child_num == 2)
        {
            //STRUCT Tag
            FieldList f = find(p_struct->children[1]->char_val);
            if(f!=NULL)
            {
                return f->type;
            }
            else
            {
                //TODO: error
            }
        }
        else
        {
            Type t = (Type)malloc(sizeof(struct Type_));
            //STRUCT OptTag LC DefList RC
            //FieldList f = (FieldList)malloc(sizeof(struct FieldList_));
            t->kind = STRUCTURE;
            char name[NAME_LEN];
            if(p_struct->children[1]==NULL)
            {
                sprintf(name, "Struct_%d",annoy_struct_num);
                annoy_struct_num += 1;
                //strncpy(f->name, name, NAME_LEN);
            }
            else
            {
                strncpy(name, p_struct->children[1]->children[0]->char_val,NAME_LEN);
            }
            t->u.structure = DefList(p->children[3]);
            if(find(name)==NULL)
            {
                insert(name,t);
            }
            else
            {
                //TODO:error
            }
            
        }
    }
    else
    {
        //Specifier: Type
        Type t = (Type)malloc(sizeof(struct Type_));
        t->kind = BASIC;
        if(strcmp(p->children[0]->name,"int")==0)
        {
            t->u.basic=INT;
        }
        else
        {
            t->u.basic=FLOAT;
        }
    }
    return t;
}