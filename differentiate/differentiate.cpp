#include "differentiate.h"
#include "../tree/tree.h"
#include "../io/io.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LEFT node->left

#define RIGHT node->right

#define DIF(node) differentiate(node, file)

#define add(left, right) create_op_node(ADD, left, right)

#define mul(left, right) create_op_node(MUL, left, right)

#define sub(left, right) create_op_node(SUB, left, right)

#define div(left, right) create_op_node(DIV, left, right)

#define pow(left, right) create_op_node(POW, left, right)

#define cos(node)        create_op_node(COS, node)

#define sin(node)        create_op_node(SIN, node)

#define ln(node)         create_op_node(LN, node)

static Node * create_op_node(OP_type op_type, double left, Node *right);

static Node * create_op_node(OP_type op_type, Node *node);

static Node * create_op_node(OP_type op_type, Node *left, Node *right);

Node * differentiate(Node *node, FILE *file) {
    assert(node != nullptr);

    Node *new_node = nullptr;

    if (node->type == NUM) {
        new_node = (Node *)calloc(1, sizeof(Node));
        assert(new_node != nullptr);
        node_init(new_node);
    
        new_node->type         = NUM;
        new_node->value.number = 0; // (const)' = 0

        return new_node;
    } else if (node->type == OP) {
        switch (node->value.op) {
            case MUL:
                new_node = add(mul(DIF(LEFT), create_cpy_node_r(RIGHT)), mul(create_cpy_node_r(LEFT), DIF(RIGHT)));
                break;
            case ADD:
                new_node = add(DIF(LEFT), DIF(RIGHT));
                break;
            case SUB:
                new_node = sub(DIF(LEFT), DIF(RIGHT));
                break;
            case DIV:
                new_node = div(sub(mul(DIF(LEFT), create_cpy_node_r(RIGHT)), mul(create_cpy_node_r(LEFT), DIF(RIGHT))), mul(create_cpy_node_r(RIGHT), create_cpy_node_r(RIGHT)));
                break;
            case SIN:
                new_node = mul(cos(create_cpy_node_r(LEFT)), DIF(LEFT));
                break;
            case COS:
                new_node = mul(mul(-1, sin(create_cpy_node_r(LEFT))), DIF(LEFT));
                break;
            case LN:
                new_node = mul(div(1, create_cpy_node_r(LEFT)), DIF(LEFT));
                break;
            case POW:
                new_node = mul(create_cpy_node_r(node), add(mul(DIF(RIGHT), ln(create_cpy_node_r(LEFT))), mul(create_cpy_node_r(RIGHT), mul(div(1, create_cpy_node_r(LEFT)), DIF(LEFT)))));
                break;
            default:
                fprintf(stderr, "node type op not exist: %d", node->value.op);
        }
    } else if (node->type == VAR) {
        new_node = (Node *)calloc(1, sizeof(Node));
        assert(new_node != nullptr);
        node_init(new_node);

        new_node->type         = NUM;
        new_node->value.number = 1; // (varible)' = 1

        return new_node;
    }

    if (file != nullptr) {
        differentiate_print(file, node, new_node);
        fprintf(file, "\n\n");
    }

    return new_node;
}

static Node * create_op_node(OP_type op_type, Node *left, Node *right) {
    assert(left  != nullptr);
    assert(right != nullptr);

    Node *node = (Node *)calloc(1, sizeof(Node));
    node_init(node);

    node->type     = OP;
    node->value.op = op_type;

    node->left  = left;
    node->right = right;

    return node;
}

static Node * create_op_node(OP_type op_type, Node *node) {
    assert(node != nullptr);

    Node *new_node = (Node *)calloc(1, sizeof(Node));
    node_init(new_node);

    new_node->type     = OP;
    new_node->value.op = op_type;

    new_node->left = node;

    return new_node;
}

static Node * create_op_node(OP_type op_type, double left, Node *right) {
    assert(right != nullptr);

    Node *node = (Node *)calloc(1, sizeof(Node));
    node_init(node);

    node->type     = OP;
    node->value.op = op_type;

    Node *double_node = (Node *)calloc(1, sizeof(Node));
    assert(double_node != nullptr);
    node_init(double_node);

    double_node->type         = NUM;
    double_node->value.number = left;

    node->left  = double_node;
    node->right = right;

    return node;
}