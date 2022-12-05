#include "tree.h"
#include <stdlib.h>
#include <assert.h>

void node_init(Node *node) {
    node->type         = (Vertex_type)0;
    node->value.number = 0;

    node->left  = nullptr;
    node->right = nullptr;
}

void del_node(Node *node) {
    if (node == nullptr) {
        return;
    }

    del_node(node->left);
    del_node(node->right);

    free(node);
}

double calc(Node *node) {
    assert(node != nullptr);

    if (node->type == NUM) {
        return node->value.number;
    }

    if (node->type == OP) {
        switch (node->value.op) {
            case ADD:
                return calc(node->left) + calc(node->right);
                break;
            case SUB:
                return calc(node->left) - calc(node->right);
                break;
            case DIV:
                return calc(node->left) / calc(node->right);
                break;
            case MUL:
                return calc(node->left) * calc(node->right);
                break;
        }
    }

    return 0;
}