#include "tree.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "../io/io.h"
#include <math.h>

static Node * mul_simp(Node *node);

static Node * add_simp(Node *node);

static Node * sub_simp(Node *node);

static Node * div_simp(Node *node);

static Node * sin_simp(Node *node);

static Node * cos_simp(Node *node);

static Node * ln_simp( Node *node);

static Node * pow_simp(Node *node);

static bool is_zero(Node *node);

static bool is_one(Node *node);

void node_init(Node *node) {
    node->type         = (Vertex_type)0;
    memset(&(node->value), 0, sizeof(node->value));

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

#define D_calc(node) calc(node, var_val)

double calc(Node *node, double var_val) {
    assert(node != nullptr);

    if (node->type == NUM) {
        return node->value.number;
    } else if (node->type == OP) {
        switch (node->value.op) {
            case ADD:
                return D_calc(node->left) + D_calc(node->right);
                break;
            case SUB:
                return D_calc(node->left) - D_calc(node->right);
                break;
            case DIV:
                return D_calc(node->left) / D_calc(node->right);
                break;
            case MUL:
                return D_calc(node->left) * D_calc(node->right);
                break;
            case SIN:
                return sin(D_calc(node->left));
                break;
            case COS:
                return cos(D_calc(node->left));
                break;
            case LN:
                return log(D_calc(node->left));
                break;
            case POW:
                return pow(D_calc(node->left), D_calc(node->right));
                break;
            default:
                fprintf(stderr, "|node type not found, type = %d|", node->type);
                break;
        }
    } else if (node->type == VAR) {
        return var_val;
    }

    return 0;
}

#define SIMP_ONE_ARGUMENT   new_node->left  = tree_simplification(node->left);\
                            assert(new_node->left  != nullptr);

#define SIMP_TWO_ARGUMENT   new_node->left  = tree_simplification(node->left);  \
                            new_node->right = tree_simplification(node->right); \
                            assert(new_node->left  != nullptr);                 \
                            assert(new_node->right != nullptr);

Node * tree_simplification(Node *node) {
    assert(node != nullptr);

    Node *new_node = create_cpy_node(node);
    assert(new_node != nullptr);

    if (node->type == OP) {
        switch (new_node->value.op) {
            case MUL:
                SIMP_TWO_ARGUMENT;
                new_node = mul_simp(new_node);
                break;
            case ADD:
                SIMP_TWO_ARGUMENT;
                new_node = add_simp(new_node);
                break;
            case SUB:
                SIMP_TWO_ARGUMENT;
                new_node = sub_simp(new_node);
                break;
            case DIV:
                SIMP_TWO_ARGUMENT;
                new_node = div_simp(new_node);
                break;
            case SIN:
                SIMP_ONE_ARGUMENT;
                new_node = sin_simp(new_node);
                break;
            case COS:
                SIMP_ONE_ARGUMENT;
                new_node = cos_simp(new_node);
                break;
            case LN:
                SIMP_ONE_ARGUMENT;
                new_node = ln_simp( new_node);
                break;
            case POW:
                SIMP_TWO_ARGUMENT;
                new_node = pow_simp(new_node);
                break;
            default:
                fprintf(stderr, "node type op not exist: %d", node->value.op);
        }
    }

    return new_node;
}

static Node * mul_simp(Node *node) {
    assert(node        != nullptr);
    assert(node->left  != nullptr);
    assert(node->right != nullptr);

    if (is_zero(node->left) || is_zero(node->right)) {
        del_node(node->left);
        del_node(node->right);

        node->type         = NUM;
        node->value.number = 0;
        node->left         = nullptr;
        node->right        = nullptr;

        return node;
    }

    if (node->left->type == NUM && node->right->type == NUM) {
        node->type         = NUM;
        node->value.number = node->left->value.number * node->right->value.number;

        del_node(node->left);
        del_node(node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return node;
    }

    if (is_one(node->left)) {
        del_node(node->left);

        Node *right = node->right;

        free(node);
        
        return right;
    }

    if (is_one(node->right)) {
        del_node(node->right);

        Node *left = node->left;

        free(node);
        
        return left;
    }

    return node;
}

static Node * add_simp(Node *node) {
    assert(node        != nullptr);
    assert(node->left  != nullptr);
    assert(node->right != nullptr);

    if (node->left->type == NUM && node->right->type == NUM) {
        node->type         = NUM;
        node->value.number = node->left->value.number + node->right->value.number;

        del_node(node->left);
        del_node(node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return node;
    }

    if (is_zero(node->left)) {
        del_node(node->left);

        Node *right = node->right;

        free(node);
        
        return right;
    }

    if (is_zero(node->right)) {
        del_node(node->right);

        Node *left = node->left;

        free(node);
        
        return left;
    }
    
    return node;
}

static Node * sub_simp(Node *node) {
    assert(node        != nullptr);
    assert(node->left  != nullptr);
    assert(node->right != nullptr);

    if (node->left->type == NUM && node->right->type == NUM) {
        node->type         = NUM;
        node->value.number = node->left->value.number - node->right->value.number;

        del_node(node->left);
        del_node(node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return node;
    }

    if (is_zero(node->right)) {
        del_node(node->right);

        Node *left = node->left;

        free(node);
        
        return left;
    }

    return node;
}

static Node * div_simp(Node *node) {
    assert(node        != nullptr);
    assert(node->left  != nullptr);
    assert(node->right != nullptr);

    if (is_zero(node->left)) {
        del_node(node->left);
        del_node(node->right);

        node_init(node);
        node->type         = NUM;
        node->value.number = 0;

        return node;
    }

    if (is_one(node->right)) {
        Node *left = node->left;

        del_node(node->right);

        free(node);

        return left;
    }

    if (node->left->type == NUM && node->right->type == NUM) {
        node->type         = NUM;
        node->value.number = node->left->value.number / node->right->value.number;

        del_node(node->left);
        del_node(node->right);
        node->left  = nullptr;
        node->right = nullptr;

        return node;
    }
    
    return node;
}

static Node * sin_simp(Node *node) {
    assert(node        != nullptr);
    assert(node->left  != nullptr);
    assert(node->right == nullptr);

    if (node->left->type == NUM) {
        node->type = NUM;
        node->value.number = sin(node->left->value.number);

        del_node(node->left);
        node->left = nullptr;

        return node;
    }

    return node;
}

static Node * cos_simp(Node *node) {
    assert(node        != nullptr);
    assert(node->left  != nullptr);
    assert(node->right == nullptr);

    if (node->left->type == NUM) {
        node->type = NUM;
        node->value.number = cos(node->left->value.number);

        del_node(node->left);
        node->left = nullptr;

        return node;
    }

    return node;
}

static Node * ln_simp(Node *node) {
    assert(node        != nullptr);
    assert(node->left  != nullptr);
    assert(node->right == nullptr);

    if (node->left->type == NUM) {
        node->type = NUM;
        node->value.number = log(node->left->value.number);

        del_node(node->left);
        node->left = nullptr;

        return node;
    }

    return node;
}

static Node * pow_simp(Node *node) {
    assert(node        != nullptr);
    assert(node->left  != nullptr);
    assert(node->right != nullptr);

    if (is_one(node->right)) {
        del_node(node->right);
        Node *left = node->left;
        free(node);

        return left;
    }

    if (is_zero(node->right)) {
        del_node(node->left);
        del_node(node->right);

        node->type         = NUM;
        node->value.number = 1;
        node->left         = nullptr;
        node->right        = nullptr;

        return node;
    }

    if (node->left->type == NUM && node->right->type == NUM) {
        node->type         = NUM;
        node->value.number = pow(node->left->value.number, node->right->value.number);

        del_node(node->left);
        del_node(node->right);

        node->left  = nullptr;
        node->right = nullptr;

        return node;
    }

    return node;
}

static bool is_zero(Node *node) {
    assert(node != nullptr);

    if (node->type == NUM) {
        if (node->value.number < EPSILON && node->value.number > -EPSILON) {
            return true;
        }
    }

    return false;
}

static bool is_one(Node *node) {
    assert(node != nullptr);

    if (node->type == NUM) {
        if (node->value.number - 1 < EPSILON && node->value.number - 1 > -EPSILON) {
            return true;
        }
    }

    return false;
}

Node * create_cpy_node(Node *node) {
    assert(node != nullptr);

    Node *new_node = (Node *)calloc(1, sizeof(Node));
    assert(new_node != nullptr);

    *new_node = *node;

    return new_node;
}

Node * create_cpy_node_r(Node * node) {
    assert(node != nullptr);

    Node *new_node = (Node *)calloc(1, sizeof(Node));
    assert(new_node != nullptr);

    *new_node = *node;

    if (new_node->left != nullptr) {
        new_node->left  = create_cpy_node_r(new_node->left);
    }
    if (new_node->right != nullptr) {
        new_node->right = create_cpy_node_r(new_node->right);
    }

    return new_node;
}