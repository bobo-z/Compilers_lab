#include "semantic.h"
int annoy_struct_num = 0;
//TODO: struct defined in a struct should not add
void semantic(Node *p)
{
    init();
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
        Type t = Specifer(p->children[0]);
        //insert(p->children[0],t);
    }
    else if (strcmp(p->children[2]->name, "SEMI") == 0)
    {
        //ExtDef: Specifier ExtDecList SEMI
        Type t = Specifer(p->children[0]);
        if (t != NULL)
        {
            ExtDecList(p->children[1], t);
        }
    }
    else
    {
        //ExtDef: Specifier FunDec CompSt
        Type t = Specifer(p->children[0]);
        FunDec(p->children[1], t);
        CompSt(p->children[2], t);
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
            FieldList f = find(p_struct->children[1]->children[0]->char_val, 0);
            if (f != NULL)
            {
                t = f->type;
                t->kind = STRUCTURE;
            }
            else
            {
                //TODO: error 17
                fprintf(stderr, "Error type 17 at Line %d: Undefined struct \"%s\".\n", p_struct->children[1]->lineno, p_struct->children[1]->children[0]->char_val);
            }
        }
        else
        {
            //STRUCT OptTag LC DefList RC
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

            //printf("%s\n", name);
            t = (Type)malloc(sizeof(struct Type_));
            t->kind = STRUCTURE;
            t->u.structure = DefList(p_struct->children[3], 1);
            Type tmp = (Type)malloc(sizeof(struct Type_));
            tmp->kind = STR_DEF;
            tmp->u.structure = t->u.structure;
            if (find(name, 0) == NULL)
            {
                insert(name, tmp);
            }
            else
            {
                //TODO:error 16
                fprintf(stderr, "Error type 16 at Line %d: Duplicated name \"%s\".\n", p_struct->children[0]->lineno, name);
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
    VarDec(p->children[0], t, 0);
    if (p->child_num == 3)
        ExtDecList(p->children[2], t);
}

FieldList VarDec(Node *p, Type t, int struct_def)
{
    FieldList f = NULL;
    if (p->child_num == 1)
    {
        //VarDec: ID
        FieldList temp = find(p->children[0]->char_val, 0);
        if (struct_def == 1 || NULL == temp)
        {
            if (struct_def == 0)
            {
                insert(p->children[0]->char_val, t);
            }
            f = (FieldList)malloc(sizeof(struct FieldList_));
            strncpy(f->name, p->children[0]->char_val, NAME_LEN);
            f->type = t;
            f->tail = NULL;
        }
        else
        {
            //TODO: error 3
            fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n", p->children[0]->lineno, p->children[0]->char_val);
        }
    }
    else
    {
        //VarDec: VarDec LB INT RB
        Type arr_t = (Type)malloc(sizeof(struct Type_));
        arr_t->kind = ARRAY;
        arr_t->u.array.elem = t;
        arr_t->u.array.size = p->children[2]->int_val;
        f = VarDec(p->children[0], arr_t, struct_def);
    }
    return f;
}

void FunDec(Node *p, Type t)
{
    if (find(p->children[0]->char_val, 1) != NULL)
    {
        //error 4
        fprintf(stderr, "Error type 4 at Line %d: Redefined function \"%s\".\n", p->children[0]->lineno, p->children[0]->char_val);
        return;
    }
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
        while (1)
        {
            Type paramdec_t = Specifer(var_p->children[0]->children[0]);
            FieldList paradec_f = VarDec(var_p->children[0]->children[1], paramdec_t, 0);
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
            if (var_p->child_num != 1)
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

void CompSt(Node *p, Type ret)
{
    //ret: return type

    DefList(p->children[1], 0);
    Node *stmtlist = p->children[2];
    while (stmtlist != NULL)
    {
        Stmt(stmtlist->children[0], ret);
        stmtlist = stmtlist->children[1];
    }
}

void Stmt(Node *p, Type ret)
{
    Type exp_t = NULL;
    switch (p->child_num)
    {
    case 1:
        //CompSt
        CompSt(p->children[0], ret);
        break;
    case 2:
        //Exp SEMI
        Exp(p->children[0]);
        break;
    case 3:
        //RETURN Exp SEMI
        exp_t = Exp(p->children[1]);
        if (Type_Check(ret, exp_t) == 0)
        {
            //False
            //TODO:error 8
            fprintf(stderr, "Error type 8 at Line %d: Type mismatched for return.\n", p->children[0]->lineno);
        }
        break;
    case 5:
        /*
        if (strcmp(p->children[0]->name, "IF") == 0)
        {
            //IF LP Exp RP Stmt
            
        }
        else
        {
            //WHILE LP Exp RP Stmt
        }
        */
        exp_t = Exp(p->children[2]);
        if (exp_t != NULL&&(exp_t->kind != BASIC||exp_t->u.basic!=BASIC_INT))
        {
            //TODO:error 7
            fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", p->children[0]->lineno);
        }
        Stmt(p->children[4], ret);
        break;
    case 7:
        //IF LP Exp RP Stmt ELSE Stmt
        exp_t = Exp(p->children[2]);
        if (exp_t != NULL&&(exp_t->kind != BASIC||exp_t->u.basic!=BASIC_INT))
        {
            //TODO:error 7
            fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", p->children[0]->lineno);
        }
        Stmt(p->children[4], ret);
        Stmt(p->children[6], ret);
        break;
    default:
        break;
    }
}

FieldList DefList(Node *p, int struct_def)
{
    FieldList ret = NULL, cur = NULL;
    while (p != NULL)
    {
        Node *def = p->children[0];
        Type def_t = Specifer(def->children[0]);
        Node *declist = def->children[1];
        while (1)
        {
            if (def_t == NULL)
                break;
            if (declist->children[0]->child_num == 1)
            {
                FieldList f = VarDec(declist->children[0]->children[0], def_t, struct_def);
                if (f != NULL)
                {
                    if (struct_def == 1 && Struct_Def_exist(ret, f->name) == 1)
                    {
                        //TODO:error 15
                        fprintf(stderr, "Error type 15 at Line %d: Redefined field \"%s\".\n", declist->children[0]->lineno, f->name);
                    }
                    else
                    {
                        //insert into fieldlist

                        if (NULL == ret)
                        {
                            ret = f;
                            cur = ret;
                        }
                        else
                        {
                            cur->tail = f;
                            cur = cur->tail;
                        }
                    }
                }
            }
            else
            {
                //Dec: VarDec ASSIGNOP Exp
                if (struct_def == 1)
                {
                    //TODO:error 15
                    fprintf(stderr, "Error type 15 at Line %d: Can not initialize fields.\n", declist->children[0]->lineno);
                }
                else
                {
                    Type exp_t = Exp(declist->children[0]->children[2]);
                    if (exp_t != NULL)
                    {
                        FieldList f = VarDec(declist->children[0]->children[0], def_t, 0);
                        if (f != NULL)
                        {
                            if (Type_Check(f->type, exp_t) == 1)
                            {
                                if (ret == NULL)
                                {
                                    ret = f;
                                    cur = ret;
                                }
                                else
                                {
                                    cur->tail = f;
                                    cur = cur->tail;
                                }
                            }
                            else
                            {
                                //error 5
                                fprintf(stderr, "Error type 5 at Line %d: Type mismatched for assignment.\n", declist->children[0]->children[1]->lineno);
                            }
                        }
                    }
                }
            }
            if (declist->child_num == 3)
            {
                declist = declist->children[2];
            }
            else
            {
                break;
            }
        }
        p = p->children[1];
    }
    return ret;
}

Type Exp(Node *p)
{
    Type t = NULL;
    Type tmp_t = NULL;
    FieldList tmp = NULL;
    char name[NAME_LEN];
    switch (p->child_num)
    {
    case 1:
        if (INT_UNIT == p->children[0]->type)
        {
            t = (Type)malloc(sizeof(struct Type_));
            t->kind = BASIC;
            t->u.basic = BASIC_INT;
        }
        else if (FLOAT_UNIT == p->children[0]->type)
        {
            t = (Type)malloc(sizeof(struct Type_));
            t->kind = BASIC;
            t->u.basic = BASIC_FLOAT;
        }
        else
        {
            tmp = find(p->children[0]->char_val, 0);
            if (NULL == tmp||tmp->type->kind==STR_DEF)
            {
                //error 1
                fprintf(stderr, "Error type 1 at Line %d: Undefined variable \"%s\".\n", p->children[0]->lineno, p->children[0]->char_val);
            }
            else
            {
                t = tmp->type;
            }
        }
        break;
    case 2:
        if (0 == strcmp(p->children[0]->name, "NOT"))
        {
            //NOT Exp
            t = Exp(p->children[1]);
            if (t != NULL)
            {
                if (t->kind != BASIC || t->u.basic != BASIC_INT)
                {
                    //error 7
                    t = NULL;
                    fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", p->children[0]->lineno);
                }
            }
        }
        else
        {
            //MINUS Exp
            t = Exp(p->children[1]);
            if (t != NULL)
            {
                if (t->kind != BASIC)
                {
                    t = NULL;
                    fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", p->children[0]->lineno);
                }
            }
        }
        break;
    case 3:
        strncpy(name, p->children[1]->name, NAME_LEN);
        if (0 == strcmp(name, "ASSIGNOP"))
        {
            //Exp: Exp ASSIGNOP Exp
            //check left value
            if (left_hand_check(p->children[0]) == 0)
            {
                //error 6
                fprintf(stderr, "Error type 6 at Line %d: The left-hand side of an assignment must be a varia-ble.\n", p->lineno);
                return NULL;
            }
            t = Exp(p->children[0]);
            tmp_t = Exp(p->children[2]);
            if (t == NULL || tmp_t == NULL)
                return NULL;
            if (Type_Check(t, tmp_t) == 0)
            {
                //error 5
                fprintf(stderr, "Error type 5 at Line %d: Type mismatched for assignment.\n", p->lineno);
                t = NULL;
            }
        }
        else if (0 == strcmp(name, "AND") || 0 == strcmp(name, "OR") || 0 == strcmp(name, "RELOP"))
        {
            //logic
            t = Exp(p->children[0]);
            tmp_t = Exp(p->children[2]);
            if (t == NULL || tmp_t == NULL)
                return NULL;
            if (t->kind == BASIC && t->u.basic == BASIC_INT)
            {
                if (1 == Type_Check(t, tmp_t))
                {
                    return t;
                }
            }
            //error 7
            //fprintf(stderr, "here!\n");
            fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", p->lineno);
            return NULL;
        }
        else if (0 == strcmp(name, "PLUS") || 0 == strcmp(name, "MINUS") || 0 == strcmp(name, "STAR") || 0 == strcmp(name, "DIV"))
        {
            //arth
            t = Exp(p->children[0]);
            tmp_t = Exp(p->children[2]);
            if (t == NULL || tmp_t == NULL)
                return NULL;
            if (t->kind == BASIC && tmp_t->kind == BASIC)
            {
                if (t->u.basic == tmp_t->u.basic)
                {
                    return t;
                }
            }
            //error 7
            fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", p->lineno);
            return NULL;
        }
        else if (0 == strcmp(name, "DOT"))
        {
            //Exp DOT ID
            t = Exp(p->children[0]);
            if (t == NULL)
                return NULL;
            if (t->kind != STRUCTURE)
            {
                //error 13
                fprintf(stderr, "Error type 13 at Line %d: Illegal use of \".\".\n", p->lineno);
                return NULL;
            }
            tmp = t->u.structure;
            while (tmp != NULL)
            {
                if (0 == strcmp(p->children[2]->char_val, tmp->name))
                {
                    return tmp->type;
                }
                tmp = tmp->tail;
            }
            //error 14
            fprintf(stderr, "Error type 14 at Line %d: Non-existent field \"%s\".\n", p->lineno, p->children[2]->char_val);
            return NULL;
        }
        else if (0 == strcmp(name, "Exp"))
        {
            //LP Exp RP
            t = Exp(p->children[1]);
        }
        else
        {
            //ID LP RP
            tmp = find(p->children[0]->char_val, 1);
            if (NULL == tmp)
            {
                tmp = find(p->children[0]->char_val, 0);
                if (NULL == tmp)
                {
                    //error 2
                    fprintf(stderr, "Error type 2 at Line %d: Undefined function \"%s\".\n", p->lineno, p->children[0]->char_val);
                }
                else
                {
                    //error 11
                    fprintf(stderr, "Error type 11 at Line %d: \"%s\" is not a function.\n", p->lineno, p->children[0]->char_val);
                }
                return NULL;
            }
            else
            {
                t = tmp->type;
                if (NULL != t->u.function.param)
                {
                    //error 9
                    fprintf(stderr, "Error type 9 at Line %d: Function arguments error.\n", p->lineno);
                    return NULL;
                }
            }
            return tmp->type->u.function.ret;
        }
        break;
    case 4:
        strncpy(name, p->children[0]->name, NAME_LEN);
        if (0 == strcmp(name, "ID"))
        {
            //ID LP Args RP
            tmp = find(p->children[0]->char_val, 1);
            if (NULL == tmp)
            {
                tmp = find(p->children[0]->char_val, 0);
                if (NULL == tmp)
                {
                    //error 2
                    fprintf(stderr, "Error type 2 at Line %d: Undefined function \"%s\".\n", p->lineno, p->children[0]->char_val);
                }
                else
                {
                    //error 11
                    fprintf(stderr, "Error type 11 at Line %d: \"%s\" is not a function.\n", p->lineno, p->children[0]->char_val);
                }
                return NULL;
            }
            else
            {
                if (0 == Args(p->children[2], tmp->type->u.function.param))
                {
                    //error 9
                    fprintf(stderr, "Error type 9 at Line %d: Function arguments error.\n", p->lineno);
                    return NULL;
                }
                else
                {
                    return tmp->type->u.function.ret;
                }
            }
        }
        else
        {
            //Exp LB Exp RB
            //a[10][3][2]
            /*
            //error 10
                fprintf(stderr,"Error type 10 at Line %d: invalid use of \"[]\" to non-array variable.\n", p->lineno);
            //error 12
                fprintf(stderr, "Error type 12 at Line %d: array index must be integer.\n",p->lineno);
            */
            t = Exp(p->children[0]);
            if (t == NULL)
                return NULL;
            if (t->kind != ARRAY)
            {
                //error 10
                fprintf(stderr, "Error type 10 at Line %d: invalid use of \"[]\" to non-array variable.\n", p->lineno);
                return NULL;
            }

            tmp_t = Exp(p->children[2]);
            if (tmp_t->kind != BASIC || tmp_t->u.basic != BASIC_INT)
            {
                //error 12
                fprintf(stderr, "Error type 12 at Line %d: array index must be integer.\n", p->lineno);
                return NULL;
            }
            t = t->u.array.elem;
            return t;
        }
        break;
    default:
        break;
    }
    return t;
}

int Args(Node *p, FieldList f)
{
    //equal 1
    //neq 0
    Type t1 = NULL;
    Type t2 = NULL;
    if (f == NULL)
        return 0;
    while (1)
    {
        t1 = Exp(p->children[0]);
        t2 = f->type;
        if (0 == Type_Check(t1, t2))
        {
            return 0;
        }
        f = f->tail;
        if (NULL == f)
        {
            if (p->child_num == 1)
            {
                return 1;
            }
            break;
        }
        else
        {
            if (p->child_num == 1)
            {
                break;
            }
            else
            {
                p = p->children[2];
            }
        }
    }
    return 0;
}

int Type_Check(Type t1, Type t2)
{
    //equal 1
    //neq 0

    if (NULL == t1 || NULL == t2)
        return 0;
    if (t1->kind != t2->kind)
        return 0;
    FieldList f1 = NULL;
    FieldList f2 = NULL;
    switch (t1->kind)
    {
    case BASIC:
        if (t1->u.basic == t2->u.basic)
        {
            return 1;
        }
        break;
    case ARRAY:
        return Type_Check(t1->u.array.elem, t2->u.array.elem);
        break;
    case STRUCTURE:
        f1 = t1->u.structure;
        f2 = t2->u.structure;
        while (f1 != NULL && f2 != NULL)
        {
            if (Type_Check(f1->type, f2->type) == 0)
            {
                return 0;
            }
            f1 = f1->tail;
            f2 = f2->tail;
        }
        if (NULL == f1 && NULL == f2)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;
    default:
        break;
    }
    return 0;
}

int Struct_Def_exist(FieldList f, char *name)
{
    while (f != NULL)
    {
        if (strcmp(f->name, name) == 0)
            return 1;
        f = f->tail;
    }
    return 0;
}

int left_hand_check(Node *p)
{
    //check the assignment on the left-hand
    //good 1
    //bad 0
    //Exp p

    if (p->child_num == 1)
    {
        //ID
        if (0 == strcmp(p->children[0]->name, "ID"))
        {
            return 1;
        }
    }
    else if (p->child_num == 3)
    {
        if (0 == strcmp(p->children[0]->name, "Exp") && 0 == strcmp(p->children[1]->name, "DOT") && 0 == strcmp(p->children[2]->name, "ID"))
        {
            return 1;
        }
    }
    else if (p->child_num == 4)
    {
        if (0 == strcmp(p->children[0]->name, "Exp") && 0 == strcmp(p->children[1]->name, "LB") && 0 == strcmp(p->children[2]->name, "Exp") && 0 == strcmp(p->children[3]->name, "RB"))
        {
            return 1;
        }
    }
    return 0;
}