#ifndef TREEH
#define TREEH

#include <stdlib.h>

const size_t MAX_VAR_SIZE = 10;

const double EPSILON      = 1e-10;

enum OP_type {
    MUL = 1,
    ADD = 2,
    SUB = 3,
    DIV = 4,
    SIN = 5,
    COS = 6,
    LN  = 7,
    POW = 8,
};

enum Vertex_type {
    OP  = 1,
    VAR = 2,
    NUM = 3,
};

union Vertex_value_type {
    OP_type     op;
    double      number;
    char        variable[MAX_VAR_SIZE];
};

struct Node {
    Vertex_type       type;
    Vertex_value_type value;

    Node *left;
    Node *right;
};

void node_init(Node *node);

void del_node(Node *node);

double calc(Node *node, double var_val = 0);

Node * tree_simplification(Node *node);

Node * create_cpy_node(Node *node);

Node * create_cpy_node_r(Node * node);

#endif