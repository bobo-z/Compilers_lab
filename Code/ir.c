#include "ir.h"

Operand ZERO = new_int_op(CONSTANT_OP,0);
Operand ONE = new_int_op(CONSTANT_OP, 1);
void ir(Node *p)
{
    if (strcmp("Program", p->name) == 0)
        translate_ExtDefList(p->children[0]);
}

void translate_ExtDefList(Node *p)
{
    if (p == NULL)
        return;
    
    translate_ExtDef(p->children[0]);
    translate_ExtDefList(p->children[1]);
}

void translate_ExtDef(Node *p)
{
    
    //Specifer FunDec Compst
    //Specifer must be int
    if (p->child_num == 2)
    {
        fprintf(stderr, "Cannot translate: Code contains variables or parameters of structure type.\n");
        exit(0);
    }
    else if (strcmp(p->children[2]->char_val, "SEMI") == 0)
        return;

    assert(0 == strcmp("TYPE", p->children[0]->children[0]->name));
    translate_FunDec(p->children[1]);
    translate_Compst(p->children[2]);
}

void translate_FunDec(Node *p)
{
    Operand func = new_char_op(FUNCTION_OP, p->children[0]->char_val);
    //assert(0);
    code_insert(new_code(1, FUNCTION_IR, func));
    if (p->child_num == 4)
    { //translate_VarList(p->children[2]);
        Node *varlist = p->children[2];
        while (1)
        {
            //PARAM t
            assert(0 == strcmp("TYPE", varlist->children[0]->children[0]->char_val));
            Node *vardec = varlist->children[0]->children[1];
            //ID or ID LB INT RB
            Operand op = new_char_op(VARIABLE_OP, vardec->children[0]->char_val);
            code_insert(new_code(1, PARAM_IR, op));

            if (varlist->child_num > 1)
                varlist = varlist->children[2];
            else
                break;
        }
    }
}

void translate_Compst(Node *p)
{
    translate_DefList(p->children[1]);
    translate_StmtList(p->children[2]);
}

void translate_DefList(Node *p)
{
    /*
DefList → Def DefList
| 
Def → Specifier DecList SEMI
DecList → Dec
| Dec COMMA DecList
Dec → VarDec
| VarDec ASSIGNOP Exp
*/
    if (p == NULL)
        return;
    Node *DecList = p->children[0]->children[1];
    while (1)
    {
        assert(0 == strcmp("TYPE", p->children[0]->children[0]->children[0]->name));
        Node *vardec = DecList->children[0]->children[0];
        while (vardec->child_num != 1)
        {

            vardec = vardec->children[0];
        }
        FieldList f = find(vardec->children[0]->char_val, 0);
        if (f != NULL && f->type->kind == ARRAY)
        {
            Operand op = new_char_op(VARIABLE_OP, f->name);
            Operand size = new_int_op(SIZE_OP, array_size(f->type));
            code_insert(new_code(2, DEC_IR, op, size));
        }
        fprintf(stderr,"%d\n",DecList->children[0]->child_num);
        if(DecList->children[0]->child_num>1)
        {
            //VarDec ASSIGNOP Exp
            Operand op = new_char_op(VARIABLE_OP, vardec->children[0]->char_val);
            Operand t1 = new_tmp();
            translate_Exp(DecList->children[0]->children[2],t1);

            code_insert(new_code(2,ASSIGN_IR,op,t1));
        }


        if (DecList->child_num > 1)
            DecList = DecList->children[2];
        else
            break;
    }
    translate_DefList(p->children[1]);
}

void translate_StmtList(Node *p)
{
}

void translate_Exp(Node* p, Operand place)
{
    if(0==strcmp("INT",p->children[0]->name))
    {
        //Exp -> INT
        Operand op = new_int_op(CONSTANT_OP, p->children[0]->int_val);
        code_insert(new_code(2, ASSIGN_IR, place, op));
        /*place ->kind = op->kind;
        place->u = op->u;*/
    }
    else if(0==strcmp("ID",p->children[0]->name))
    {
        //Exp -> ID
        Operand op = new_char_op(VARIABLE_OP, p->children[0]->char_val);
        code_insert(new_code(2, ASSIGN_IR, place, op));
        /*
        place ->kind = op->kind;
        place->u = op->u;
        */
    }
    else if(p->child_num>1&&(0==strcmp("ASSIGNOP",p->children[1]->name)))
    {
        Operand t1 = new_tmp();
        Operand t2 = new_tmp();
        translate_Exp(p->children[0],t1);
        translate_Exp(p->children[2],t2);
        code_insert(new_code(2,ASSIGN_IR, t1, t2));
    }
    else if(p->child_num>1&&(0==strcmp("PLUS",p->children[1]->name)))
    {
        Operand t1 = new_tmp();
        Operand t2 = new_tmp();
        translate_Exp(p->children[0],t1);
        translate_Exp(p->children[2],t2);
        code_insert(new_code(3, ADD_IR, place, t1, t2));
    }
    else if(p->child_num>1&&(0==strcmp("MINUS",p->children[1]->name)))
    {
        Operand t1 = new_tmp();
        Operand t2 = new_tmp();
        translate_Exp(p->children[0],t1);
        translate_Exp(p->children[2],t2);
        code_insert(new_code(3, SUB_IR, place, t1, t2));
    }
    else if(p->child_num>1&&(0==strcmp("STAR",p->children[1]->name)))
    {
        Operand t1 = new_tmp();
        Operand t2 = new_tmp();
        translate_Exp(p->children[0],t1);
        translate_Exp(p->children[2],t2);
        code_insert(new_code(3, MUL_IR, place, t1, t2));
    }
    else if(p->child_num>1&&(0==strcmp("DIV",p->children[1]->name)))
    {
        Operand t1 = new_tmp();
        Operand t2 = new_tmp();
        translate_Exp(p->children[0],t1);
        translate_Exp(p->children[2],t2);
        code_insert(new_code(3, DIV_IR, place, t1, t2));
    }
    else if(p->child_num>1&&(0==strcmp("MINUS",p->children[0]->name)))
    {
        //Exp -> MINUS Exp
        Operand t1 = new_tmp();
        translate_Exp(p->children[1],t1);
        code_insert(new_code(3,SUB_IR, place, ZERO, t1));
    }
    else if(p->child_num>1&&(0==strcmp("RELOP",p->children[1]->name)||0==strcmp("NOT",p->children[0]->name)||0==strcmp("AND",p->children[1]->name)||0==strcmp("OR",p->children[1]->name)))
    {
        Operand label1 = new_label();
        Operand label2 = new_label();
        
        code_insert(new_code(2,ASSIGN_IR, place, ZERO));
        translate_Cond(p,label1,label2);
        code_insert(new_code(1, LABEL_IR, label1));
        code_insert(new_code(2, ASSIGN_IR, place, ONE));


    }

}

int array_size(Type t)
{
    if (t != NULL && t->kind != ARRAY)
        return 4;
    else
        return t->u.array.size * array_size(t->u.array.elem);
}