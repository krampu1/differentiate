#include "teylor_row.h"
#include "../differentiate/differentiate.h"
#include <assert.h>

Node * teylor_row(Node *node, double var_val) {
    assert(node != nullptr);

    Node *diff_one = differentiate(node);
    assert(diff_one != nullptr);

    Node *diff_two = differentiate(diff_one);
    assert(diff_two != nullptr);

    Node *new_node = (Node *)calloc(1, sizeof(Node));
    node_init(new_node);

    new_node->type     = OP;
    new_node->value.op = ADD;
    new_node->left     = diff_one;
    new_node->right    = diff_two;

    return new_node;
}