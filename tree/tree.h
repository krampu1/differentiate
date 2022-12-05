#ifndef TREEH
#define TREEH

enum OP_type {
    MUL = 1,
    ADD = 2,
    SUB = 3,
    DIV = 4,
};

enum Vertex_type {
    OP  = 1,
    VAR = 2,
    NUM = 3,
};

union Vertex_value_type {
    OP_type     op;
    double      number;
    const char *variable;
};

struct Node {
    Vertex_type       type;
    Vertex_value_type value;

    Node *left;
    Node *right;
};

void node_init(Node *node);

void del_node(Node *node);

double calc(Node *node);

#endif