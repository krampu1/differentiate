#include "differentiate.h"
#include "../tree/tree.h"
#include "../io/io.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LEFT node->left

#define RIGHT node->right

#define DIF(node) diffirentiate(node, file)

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

Node * create_cpy_node(Node *node);

Node * diffirentiate(Node *node, FILE *file = nullptr) {
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
                new_node = add(mul(DIF(LEFT), RIGHT), mul(LEFT, DIF(RIGHT)));
                break;
            case ADD:
                new_node = add(DIF(LEFT), DIF(RIGHT));
                break;
            case SUB:
                new_node = sub(DIF(LEFT), DIF(RIGHT));
                break;
            case DIV:
                new_node = div(sub(mul(DIF(LEFT), RIGHT), mul(LEFT, DIF(RIGHT))), mul(RIGHT, RIGHT));
                break;
            case SIN:
                new_node = cos(LEFT);
                break;
            case COS:
                new_node = mul(-1, sin(node));
                break;
            case LN:
                new_node = div(1, LEFT);
                break;
            case POW:
                new_node = mul(node, DIF(mul(RIGHT, ln(LEFT))));
                break;
            default:
                printf("node type op not exist: %d", node->value.op);
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

    node->left  = create_cpy_node(left);
    node->right = create_cpy_node(right);

    return node;
}

static Node * create_op_node(OP_type op_type, Node *node) {
    assert(node != nullptr);

    Node *new_node = (Node *)calloc(1, sizeof(Node));
    node_init(new_node);

    new_node->type     = OP;
    new_node->value.op = op_type;

    new_node->left = create_cpy_node(node);

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
    node->right = create_cpy_node(right);

    return node;
}

Node * create_cpy_node(Node *node) {
    assert(node != nullptr);

    Node *new_node = (Node *)calloc(1, sizeof(Node));
    assert(new_node != nullptr);

    *new_node = *node;

    return new_node;
}