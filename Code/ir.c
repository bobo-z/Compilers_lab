#include "ir.h"

Operand ZERO = NULL;
Operand ONE = NULL;
Operand FOUR = NULL;
void ir(Node *p)
{
    ZERO = new_int_op(CONSTANT_OP, 0);
    ONE = new_int_op(CONSTANT_OP, 1);
    FOUR = new_int_op(CONSTANT_OP, 4);
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
            //fprintf(stderr, "%s\n", varlist->children[0]->children[0]->children[0]->name);
            assert(0 == strcmp("TYPE", varlist->children[0]->children[0]->children[0]->name));

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
        if (DecList->children[0]->child_num > 1)
        {
            //VarDec ASSIGNOP Exp
            //fprintf(stderr, "3\n");
            Operand op = new_char_op(VARIABLE_OP, vardec->children[0]->char_val);
            Operand t1 = new_tmp();
            translate_Exp(DecList->children[0]->children[2], t1);

            code_insert(new_code(2, ASSIGN_IR, op, t1));
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
    //StmtList->empty
    if (p == NULL)
        return;
    //StmtList->Stmt StmtList
    translate_Stmt(p->children[0]);
    translate_StmtList(p->children[1]);
}

void translate_Stmt(Node *p)
{
    if (p->child_num == 1)
    {
        //Stmt -> CompSt
        translate_Compst(p->children[0]);
    }
    else if (p->child_num == 2)
    {
        //Stmt-> Exp SEMI
        translate_Exp(p->children[0], NULL);
    }
    else if (p->child_num == 3)
    {
        //Stmt -> RETURN Exp SEMI
        Operand t1 = new_tmp();
        translate_Exp(p->children[1], t1);
        code_insert(new_code(1, RETURN_IR, t1));
    }
    else if (p->child_num == 5)
    {
        if (0 == strcmp(p->children[0]->name, "IF"))
        {
            //Stmt -> IF LP Exp RP Stmt
            Operand label1 = new_label();
            Operand label2 = new_label();
            translate_Cond(p->children[2], label1, label2);
            code_insert(new_code(1, LABEL_IR, label1));
            translate_Stmt(p->children[4]);
            code_insert(new_code(1, LABEL_IR, label2));
        }
        else
        {
            //Stmt -> WHILE LP Exp RP Stmt
            Operand label1 = new_label();
            Operand label2 = new_label();
            Operand label3 = new_label();
            code_insert(new_code(1, LABEL_IR, label1));
            translate_Cond(p->children[2], label2, label3);
            code_insert(new_code(1, LABEL_IR, label2));
            translate_Stmt(p->children[4]);
            code_insert(new_code(1, GOTO_IR, label1));
            code_insert(new_code(1, LABEL_IR, label3));
        }
    }
    else if (p->child_num == 7)
    {
        //Stmt -> IF LP Exp RP Stmt ELSE Stmt
        Operand label1 = new_label();
        Operand label2 = new_label();
        Operand label3 = new_label();
        translate_Cond(p->children[2], label1, label2);
        code_insert(new_code(1, LABEL_IR, label1));
        translate_Stmt(p->children[4]);
        code_insert(new_code(1, GOTO_IR, label3));
        code_insert(new_code(1, LABEL_IR, label2));
        translate_Stmt(p->children[6]);
        code_insert(new_code(1, LABEL_IR, label3));
    }
}

void translate_Exp(Node *p, Operand place)
{
    if (0 == strcmp("INT", p->children[0]->name))
    {
        //Exp -> INT
        Operand op = new_int_op(CONSTANT_OP, p->children[0]->int_val);
        code_insert(new_code(2, ASSIGN_IR, place, op));
        /*place ->kind = op->kind;
        place->u = op->u;*/
    }
    else if (p->child_num == 1 && 0 == strcmp("ID", p->children[0]->name))
    {
        //Exp -> ID
        Operand op = new_char_op(VARIABLE_OP, p->children[0]->char_val);
        code_insert(new_code(2, ASSIGN_IR, place, op));
        /*
        place ->kind = op->kind;
        place->u = op->u;
        */
    }
    else if (p->child_num > 1 && (0 == strcmp("ASSIGNOP", p->children[1]->name)))
    {
        Operand left = NULL, right = NULL;
        if (0 == strcmp(p->children[0]->children[0]->name, "ID"))
        {
            Operand t1 = new_tmp();
            left = new_char_op(VARIABLE_OP, p->children[0]->children[0]->char_val);
        }
        else
        {
            //Exp-> Exp1 ASSIGN Exp
            //Exp1 -> Exp LB Exp RB
            Operand var = new_char_op(MEMTOADDR_OP, p->children[0]->children[0]->children[0]->char_val);
            Operand t1 = new_tmp();
            translate_Exp(p->children[0]->children[2], t1);
            Operand t2 = new_tmp();
            code_insert(new_code(3, MUL_IR, t2, t1, FOUR));
            code_insert(new_code(3, ADD_IR, t1, var, t2));
            left = new_char_op(ADDRTOMEM_OP, t1->u.char_value);
        }
        Operand t3 = new_tmp();
        translate_Exp(p->children[2], t3);
        right = t3;
        code_insert(new_code(2, ASSIGN_IR, left, right));
    }
    else if (p->child_num > 1 && (0 == strcmp("PLUS", p->children[1]->name)))
    {
        Operand t1 = new_tmp();
        Operand t2 = new_tmp();
        translate_Exp(p->children[0], t1);
        translate_Exp(p->children[2], t2);
        code_insert(new_code(3, ADD_IR, place, t1, t2));
    }
    else if (p->child_num > 1 && (0 == strcmp("MINUS", p->children[1]->name)))
    {
        Operand t1 = new_tmp();
        Operand t2 = new_tmp();
        translate_Exp(p->children[0], t1);
        translate_Exp(p->children[2], t2);
        code_insert(new_code(3, SUB_IR, place, t1, t2));
    }
    else if (p->child_num > 1 && (0 == strcmp("STAR", p->children[1]->name)))
    {
        Operand t1 = new_tmp();
        Operand t2 = new_tmp();
        translate_Exp(p->children[0], t1);
        translate_Exp(p->children[2], t2);
        code_insert(new_code(3, MUL_IR, place, t1, t2));
    }
    else if (p->child_num > 1 && (0 == strcmp("DIV", p->children[1]->name)))
    {
        Operand t1 = new_tmp();
        Operand t2 = new_tmp();
        translate_Exp(p->children[0], t1);
        translate_Exp(p->children[2], t2);
        code_insert(new_code(3, DIV_IR, place, t1, t2));
    }
    else if (p->child_num > 1 && (0 == strcmp("MINUS", p->children[0]->name)))
    {
        //Exp -> MINUS Exp
        Operand t1 = new_tmp();
        translate_Exp(p->children[1], t1);
        code_insert(new_code(3, SUB_IR, place, ZERO, t1));
    }
    else if (p->child_num > 1 && (0 == strcmp("RELOP", p->children[1]->name) || 0 == strcmp("NOT", p->children[0]->name) || 0 == strcmp("AND", p->children[1]->name) || 0 == strcmp("OR", p->children[1]->name)))
    {
        Operand label1 = new_label();
        Operand label2 = new_label();

        code_insert(new_code(2, ASSIGN_IR, place, ZERO));
        translate_Cond(p, label1, label2);
        code_insert(new_code(1, LABEL_IR, label1));
        code_insert(new_code(2, ASSIGN_IR, place, ONE));
        code_insert(new_code(1, LABEL_IR, label2));
    }
    else if (p->child_num == 3 && 0 == strcmp("ID", p->children[0]->name))
    {
        //Exp -> ID LP RP
        if (0 == strcmp(p->children[0]->char_val, "read"))
        {
            code_insert(new_code(1, READ_IR, place));
        }
        else
        {
            Operand func = new_char_op(FUNCTION_OP, p->children[0]->char_val);
            if (place != NULL)
                code_insert(new_code(2, CALL_IR, place, func));
        }
    }
    else if (p->child_num == 4 && 0 == strcmp("ID", p->children[0]->name))
    {
        ArgList arg_list = NULL;
        arg_list = translate_Args(p->children[2], arg_list);
        if (0 == strcmp(p->children[0]->char_val, "write"))
        {

            code_insert(new_code(1, WRITE_IR, arg_list->op));
            if (place != NULL)
                code_insert(new_code(2, ASSIGN_IR, place, ZERO));
        }
        else
        {
            ArgList cur = arg_list;
            while (cur != NULL)
            {
                code_insert(new_code(1, ARG_IR, cur->op));
                cur = cur->next;
            }
            Operand func = new_char_op(FUNCTION_OP, p->children[0]->char_val);
            if (place != NULL)
                code_insert(new_code(2, CALL_IR, place, func));
        }
    }
    else if (p->child_num > 1 && 0 == strcmp("LP", p->children[0]->name))
    {
        translate_Exp(p->children[1], place);
    }
    else if (p->child_num > 1 && 0 == strcmp("LB", p->children[1]->name))
    {
        //Exp -> Exp(ID) LB Exp RB
        if(place == NULL)
            return;
        Operand var = new_char_op(MEMTOADDR_OP, p->children[0]->children[0]->char_val);
        Operand t1 = new_tmp();
        translate_Exp(p->children[2], t1);
        //code_insert
        Operand t2 = new_tmp();
        code_insert(new_code(3, MUL_IR, t2, t1, FOUR));
        code_insert(new_code(3, ADD_IR, t1, var, t2));
        Operand t3 = new_tmp();
        t3->kind = ADDRTOMEM_OP;
        strncpy(t3->u.char_value, t1->u.char_value, NAME_LEN);
        code_insert(new_code(2, ASSIGN_IR, place, t3));
    }
}

ArgList translate_Args(Node *p, ArgList head)
{
    //ArgList tail = NULL;
    while (1)
    {
        Operand t1 = new_tmp();
        translate_Exp(p->children[0], t1);
        ArgList arg = (ArgList)malloc(sizeof(struct ArgList_));
        arg->op = t1;
        arg->next = NULL;
        if (head == NULL)
        {
            head = arg;
            //tail = head;
        }
        else
        {
            arg->next = head;
            head = arg;
            //tail->next = arg;
            //tail = arg;
        }

        if (p->child_num == 1)
            break;
        else
            p = p->children[2];
    }
    return head;
}

void translate_Cond(Node *p, Operand label_true, Operand label_false)
{
    if (0 == strcmp("NOT", p->children[0]->name))
    {
        //Exp -> NOT Exp
        translate_Cond(p->children[1], label_false, label_true);
    }
    else if (p->child_num > 1 && 0 == strcmp("RELOP", p->children[1]->name))
    {
        //Exp -> Exp RELOP Exp
        Operand t1 = new_tmp();
        Operand t2 = new_tmp();
        translate_Exp(p->children[0], t1);
        translate_Exp(p->children[2], t2);
        Operand op = new_char_op(RELOP_OP, p->children[1]->char_val);
        code_insert(new_code(4, IF_IR, t1, op, t2, label_true));
        code_insert(new_code(1, GOTO_IR, label_false));
    }
    else if (p->child_num > 1 && 0 == strcmp("AND", p->children[1]->name))
    {
        //Exp -> Exp AND Exp
        Operand label1 = new_label();
        translate_Cond(p->children[0], label1, label_false);
        code_insert(new_code(1, LABEL_IR, label1));
        translate_Cond(p->children[2], label_true, label_false);
    }
    else if (p->child_num > 1 && 0 == strcmp("OR", p->children[1]->name))
    {
        Operand label1 = new_label();
        translate_Cond(p->children[0], label_true, label1);
        code_insert(new_code(1, LABEL_IR, label1));
        translate_Cond(p->children[2], label_true, label_false);
    }
    else
    {
        Operand t1 = new_tmp();
        translate_Exp(p, t1);
        Operand op = new_char_op(RELOP_OP, "!=");
        code_insert(new_code(4, IF_IR, t1, op, ZERO, label_true));
        code_insert(new_code(1, GOTO_IR, label_false));
    }
}

int array_size(Type t)
{
    if (t != NULL && t->kind != ARRAY)
        return 4;
    else
        return t->u.array.size * array_size(t->u.array.elem);
}