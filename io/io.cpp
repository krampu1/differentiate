#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../string/string.h"
#include "io.h"
#include <string.h>
#include <assert.h>
#include "../tree/tree.h"
#include <cctype>

static Node * read_add_sub(const char **ptr);

static Node * read_mul_div(const char **ptr);

static Node * read_brackets_func(const char **ptr);

static Node * read_number_var(const char **ptr);

static void _case_operator(const char *op);

static Node * read_pow(const char **ptr);

static size_t get_file_size(FILE * ptr_file) {
    assert(ptr_file != nullptr);

    struct stat fileinfo = {};
    fstat(fileno(ptr_file), &fileinfo);

    return fileinfo.st_size;
}

size_t get_data_file(char **buff, const char *file_path) {
    assert(buff      != nullptr);
    assert(file_path != nullptr);


    FILE * ptrFile = fopen(file_path, "rb");
    assert(ptrFile != nullptr);

    size_t file_size = get_file_size(ptrFile);

    *buff = (char *)calloc(file_size + 2, sizeof(**buff));

    assert(buff != nullptr);

    fread(*buff, sizeof(char), file_size, ptrFile);
    (*buff)[file_size + 1] = (*buff)[file_size] = 0;

    fclose(ptrFile);

    return file_size;
}

size_t buff_to_text(KR_string **text, char *buff, size_t buff_size) {
    assert(text != nullptr);
    assert(buff != nullptr);

    size_t text_size = 1;

    for (size_t i = 0; i < buff_size; i++) {
        if (buff[i] == '\n') {
            text_size++;
        }
    }
    
    *text = (KR_string *)calloc(text_size + 1, sizeof(**text));
    
    assert(text != nullptr);

    ((*text)[text_size]).ptr = nullptr;

    ((*text)[0]).ptr = buff;
    
    size_t ptr = 1;

    KR_string *ptr_to_last = *text;

    assert(ptr_to_last != nullptr);

    for (size_t i = 0; i < buff_size + 1; i++) {
        if (ptr_to_last != nullptr && (buff[i] == '\r' || buff[i] == '\n' || buff[i] == 0)) {
            (*ptr_to_last).ptr_end = &buff[i];

            ptr_to_last = nullptr;
        }

        if (buff[i] == '\r') {
            buff[i] = 0;
        }
        if (i > 0 && buff[i - 1] == '\n') {
            buff[i - 1] = 0;

            ptr_to_last = &((*text)[ptr]);

            (*text)[ptr++].ptr = &buff[i];
        }
    }
    if (ptr_to_last != nullptr) {
        (*ptr_to_last).ptr_end = &buff[buff_size];
    }

    for (size_t i = 0; i < text_size; i++) {
        assert((*text)[i].ptr     != nullptr);
        assert((*text)[i].ptr_end != nullptr);
    }

    return text_size;
}

void get_file_name_from_flug(const char **file_path, int argc, char *argv[]) {
    //åñëè åñòü íàçâàíèå ôàéëà ââîäà òî ôëàã -f òî÷íî íå ïîñëåäíèé, çíà÷èò ïåðåáèðàòü argc - 1 áåçñìûñëåííî
    //à òàê æå ýòî ïîìîæåò èçáåæàòü îøèáêè åñëè ôëàã óêàçàëè ïîñëåäíèì
    for (size_t i = 0; i < argc - 1; i++) {
        if (!strncmp(argv[i], "-f", 3)) {
            *file_path = argv[i + 1];
            break;
        }
    }
}

void fprintf_buff(FILE *ptrfileout, char *buff, size_t buff_size) {
    assert(ptrfileout != nullptr);
    assert(buff       != nullptr);

    for (size_t i = 0; i < buff_size; i++) {
        if (buff[i] == '\0') {
            if (buff[i + 1] != '\0') {
                fprintf(ptrfileout, "\n");
            }
        }
        else {
            fprintf(ptrfileout, "%c", buff[i]);
        }
    }
}

size_t get_text_file(KR_string **text, char **buff, size_t *buff_size, const char *file_path) {
    assert(text      != nullptr);
    assert(buff      != nullptr);
    assert(buff_size != nullptr);

    *buff_size = get_data_file(buff, file_path);

    assert(buff != nullptr);

    size_t text_size = buff_to_text(text, *buff, *buff_size);

    assert(text != nullptr);

    return text_size;
}

void dell_speces(char *buff) {
    char *r = buff;
    char *l = buff;

    while (*r != '\n' && *r != '\r' && *r != '\0') {
        if (!isspace(*r)) {
            *l = *r;
            l++;
        }
        r++;
    }

    *l = '\0';
}

Node * buff_to_tree(const char *ptr) {
    Node *node = read_add_sub(&ptr);
    assert(*ptr == '\0');

    return node;
}

static Node * read_add_sub(const char **ptr) {
    assert(ptr != nullptr);
    assert(*ptr != nullptr);

    Node *node  = read_mul_div(ptr);
    assert(node != nullptr);

    while (**ptr == '+' || **ptr == '-') {
        char op = **ptr;
        (*ptr)++;

        Node *added_node = read_mul_div(ptr);
        assert(added_node != nullptr);

        Node *new_node = (Node *)calloc(1, sizeof(Node));
        assert(new_node != nullptr);

        new_node->type = OP;

        if (op == '+') {
            new_node->value.op = ADD;
        }

        if (op == '-') {
            new_node->value.op = SUB;
        }

        new_node->left  = node;
        new_node->right = added_node;

        node = new_node;
    }
    
    return node;
}

static Node * read_mul_div(const char **ptr) {
    assert(ptr != nullptr);
    assert(*ptr != nullptr);

    Node *node  = read_pow(ptr);
    assert(node != nullptr);

    while (**ptr == '*' || **ptr == '/') {
        char op = **ptr;
        (*ptr)++;

        Node *added_node = read_mul_div(ptr);
        assert(added_node != nullptr);

        Node *new_node = (Node *)calloc(1, sizeof(Node));
        assert(new_node != nullptr);

        new_node->type = OP;

        if (op == '*') {
            new_node->value.op = MUL;
        }

        if (op == '/') {
            new_node->value.op = DIV;
        }

        new_node->left  = node;
        new_node->right = added_node;

        node = new_node;
    }
    
    return node;
}

static Node * read_pow(const char **ptr) {
    assert(ptr != nullptr);
    assert(*ptr != nullptr);

    Node *node  = read_brackets_func(ptr);
    assert(node != nullptr);

    while (**ptr == '^') {
        char op = **ptr;
        (*ptr)++;

        Node *added_node = read_pow(ptr);
        assert(added_node != nullptr);

        Node *new_node = (Node *)calloc(1, sizeof(Node));
        assert(new_node != nullptr);

        new_node->type = OP;
        new_node->value.op = POW;

        new_node->left  = node;
        new_node->right = added_node;

        node = new_node;
    }

    return node;
}

#define read_function(func_name, func_type) (strncmp(*ptr, (func_name), sizeof((func_name)) - 1) == 0) {\
                                                *ptr += sizeof((func_name)) - 1;                        \
                                                                                                        \
                                                node = (Node *)calloc(1, sizeof(Node));                 \
                                                assert(node != nullptr);                                \
                                                node_init(node);                                        \
                                                                                                        \
                                                node->type = OP;                                        \
                                                node->value.op = func_type;                             \
                                                                                                        \
                                                node->left = read_add_sub(ptr);                         \
                                                                                                        \
                                                assert(**ptr == ')');                                   \
                                                (*ptr)++;                                               \
                                            }

static Node * read_brackets_func(const char **ptr) {
    assert(ptr != nullptr);
    assert(*ptr != nullptr);

    Node *node = nullptr;

    if (**ptr == '(') {
        (*ptr)++;

        node = read_add_sub(ptr);
        assert(node != nullptr);

        assert(**ptr == ')');
        (*ptr)++;
    }
    else if read_function("sin(", SIN)
    else if read_function("cos(", COS)
    else if read_function("ln(",  LN)
    else {
        node = read_number_var(ptr);
    }
    
    return node;
}

static Node * read_number_var(const char **ptr) {
    assert(ptr != nullptr);
    assert(*ptr != nullptr);

    Node *node = (Node *)calloc(1, sizeof(Node));
    assert(node != nullptr);
    node_init(node);

    if (isdigit(**ptr)) {
        node->type         = NUM;
        node->value.number = 0;

        node->value.number = **ptr - '0';
        (*ptr)++;
        
        while (isdigit(**ptr)) {
            node->value.number *= 10;
            node->value.number += **ptr - '0';
            (*ptr)++;
        }        
    } else {
        node->type  = VAR;
        char *var_ptr = node->value.variable;

        while (isalpha(**ptr) || isdigit(**ptr)) {
            assert((var_ptr - node->value.variable) < MAX_VAR_SIZE);
            *var_ptr = **ptr;
            var_ptr++;
            (*ptr)++;
        }
        
        assert((var_ptr - node->value.variable) < MAX_VAR_SIZE);
        *var_ptr = 0;

        
    }
    
    return node;
}

static void _case_operator(const char *op, Node *node) {
    assert(node != nullptr);

    printf("(");
    print_tree(node->left);
    printf("%s", op);
    print_tree(node->right);
    printf(")");
}

static void _case_func(const char *func, Node *node) {
    assert(node != nullptr);

    printf("%s(", func);
    print_tree(node->left);
    printf(")");
}

#define case_operator(op, op_type, node)    case op_type:                 \
                                                _case_operator(op, node); \
                                                break;

#define case_func(func, func_type, node)    case func_type :              \
                                                _case_func(func, node);   \
                                                break;

void print_tree(Node *node) {
    assert(node != nullptr);

    if (node->type == NUM) {
        printf("%lg", node->value.number);
    } else if (node->type == OP) {
        switch (node->value.op) {
            case_operator("*", MUL, node);
            case_operator("+", ADD, node);
            case_operator("-", SUB, node);
            case_operator("/", DIV, node);
            case_operator("^", POW, node);

            case_func("sin", SIN, node);
            case_func("cos", COS, node);
            case_func("ln",  LN,  node);
            
            default:
                printf("!node type op not exist: %d!", node->value.op);
        }
    } else if (node->type == VAR) {
        printf("%s", node->value.variable);
    } else {
        printf("error");
    }
}