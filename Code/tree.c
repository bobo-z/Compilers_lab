#include "common.h"

Node *InitNode(char *name, int lineno, int type)
{
    Node *node = (Node *)malloc(sizeof(Node));
    strcpy(node->name, name);
    node->lineno = lineno;
    assert(type >= 0 && type <= 4);
    node->type = type;
    node->int_val = node->float_val = 0;
    node->char_val = "\0";
    node->children = NULL;
    node->child_num = 0;

    return node;
}

/* refer to https://www.cnblogs.com/edver/p/8419807.html */
void AddChild(Node *father, int num, ...)
{
    va_list vl;
    va_start(vl, num);
    father->child_num = num;
    father->children = (Node **)malloc(num * sizeof(Node *));
    int i;
    for (i = 0; i < num; i++)
    {
        father->children[i] = va_arg(vl, Node *);
    }
    va_end(vl);
}

void PrintTree(Node *cur_node, int depth)
{
    if (cur_node == NULL)
        return;
    int i;
    for (i = 0; i < depth; i++)
        printf("  ");

    switch (cur_node->type)
    {
    case SYNTAX_UNIT:
        printf("%s (%d)\n", cur_node->name, cur_node->lineno);
        break;
    case INT_UNIT:
        printf("%s: %d\n", cur_node->name, cur_node->int_val);
        break;
    case FLOAT_UNIT:
        printf("%s: %f\n", cur_node->name, cur_node->float_val);
        break;
    case CHAR_UNIT:
        printf("%s: %s\n", cur_node->name, cur_node->char_val);
        break;
    case OTHER_UNIT:
        printf("%s\n", cur_node->name);
        break;
    default:
        assert(0);
        break;
    }

    for (i = 0; i < cur_node->child_num; i++)
    {
        PrintTree(cur_node->children[i], depth + 1);
    }
}