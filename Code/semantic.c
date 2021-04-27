#include "semantic.h"
int annoy_struct_num = 0;

void semantic(Node *p)
{
    if (strcmp(p->name, "Program") == 0)
        ExtDefList(p->children[0]);
}

void ExtDefList(Node *p)
{
    if (p == NULL)
    {
        return;
    }
    else
    {
        ExtDef(p->children[0]);
        ExtDefList(p->children[1]);
    }
}

void ExtDef(Node *p)
{
    if (p->child_num == 2)
    {
        //ExtDef: Specifer SEMI
        Specifer(p->children[0]);
    }
    else if (strcmp(p->children[2]->name, "SEMI") == 0)
    {
        //ExtDef: Specifier ExtDecList SEMI
        Type t = Specifer(p->children[0]);
        ExtDecList(p->children[1], t);
    }
    else
    {
        //ExtDef: Specifier FunDec CompSt
        Type t = Specifer(p->children[0]);
        FunDec(p->children[1], t);
        CompSt(p->children[2]);
    }
}

Type Specifer(Node *p)
{
    Type t = NULL;
    if (p->children[0]->type == SYNTAX_UNIT)
    {
        //Specifer: StructSpecifier

        Node *p_struct = p->children[0]; //StructSpecifer
        if (p_struct->child_num == 2)
        {
            //STRUCT Tag
            FieldList f = find(p_struct->children[1]->char_val, 0);
            if (f != NULL)
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
            t = (Type)malloc(sizeof(struct Type_));
            //STRUCT OptTag LC DefList RC
            //FieldList f = (FieldList)malloc(sizeof(struct FieldList_));
            t->kind = STRUCTURE;
            char name[NAME_LEN];
            if (p_struct->children[1] == NULL)
            {
                sprintf(name, "Struct_%d", annoy_struct_num);
                annoy_struct_num += 1;
                //strncpy(f->name, name, NAME_LEN);
            }
            else
            {
                strncpy(name, p_struct->children[1]->children[0]->char_val, NAME_LEN);
            }
            t->u.structure = DefList(p->children[3]);
            if (find(name, 0) == NULL)
            {
                insert(name, t);
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
        t = (Type)malloc(sizeof(struct Type_));
        t->kind = BASIC;
        if (strcmp(p->children[0]->char_val, "int") == 0)
        {
            t->u.basic = BASIC_INT;
        }
        else
        {
            t->u.basic = BASIC_FLOAT;
        }
    }
    return t;
}

void ExtDecList(Node *p, Type t)
{
    VarDec(p->children[0], t);
    if (p->child_num == 3)
        ExtDecList(p->children[2], t);
}

FieldList VarDec(Node *p, Type t)
{
    if (p->child_num == 1)
    {
        //VarDec: ID
        if (find(p->children[0]->char_val, 0) == NULL)
        {
            insert(p->children[0]->char_val, t);
            FieldList f = (FieldList)malloc(sizeof(struct FieldList_));
            strncpy(f->name, p->children[0]->char_val, NAME_LEN);
            f->type = t;
            f->tail = NULL;
            return f;
        }
        else
        {
            //TODO: error
            return NULL;
        }
    }
    else
    {
        //VarDec: VarDec LB INT RB
        Type arr_t = (Type)malloc(sizeof(struct Type_));
        arr_t->kind = ARRAY;
        arr_t->u.array.elem = t;
        arr_t->u.array.size = p->children[2]->int_val;
        VarDec(p->children[0], arr_t);
    }
}

void FunDec(Node *p, Type t)
{
    if(find(p->children[0]->char_val,1)!=NULL)
        return;
    Type fun_t = (Type)malloc(sizeof(struct Type_));
    fun_t->kind = FUNCTION;
    fun_t->u.function.ret = t;
    fun_t->u.function.param = NULL;
    FieldList cur = NULL;
    if (p->child_num != 3)
    {
        //FunDec: ID LP VarList RP
        Node *var_p = p->children[2];
        //ParamDec
        while(1)
        {
            Type paramdec_t = Specifer(var_p->children[0]->children[0]);
            FieldList paradec_f = VarDec(var_p->children[0]->children[1], paramdec_t);
            if (fun_t->u.function.param == NULL)
            {
                fun_t->u.function.param = paradec_f;
                cur = fun_t->u.function.param;
            }
            else
            {
                cur->tail = paradec_f;
                cur = cur->tail;
            }
            if(var_p->child_num!=1)
            {
                var_p = var_p->children[2];
            }
            else
            {
                break;
            }
        }
    }
    insert(p->children[0]->char_val, fun_t);
}

void CompSt(Node *p)
{
}

FieldList DefList(Node *p)
{
}