#ifndef TREE_H_
#define TREE_H_

/* Type */
#define SYNTAX_UNIT 0
#define INT_UNIT 1
#define FLOAT_UNIT 2
#define CHAR_UNIT 3
#define OTHER_UNIT 4//tokens except int, float, id, type

typedef struct node
{
    char *name;
    int lineno;
    int type;
    union
    {
        int int_val;
        float float_val;
        char *char_val;
    };
    struct node **children;
    int child_num;
} Node;

void AddChild(Node *father, int num, ...);
Node *InitNode(char *name, int lineno, int type);
void PrintTree(Node* cur_node, int depth);

#endif