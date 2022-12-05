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

static Node * read_brackets(const char **ptr);

static Node * read_number(const char **ptr);

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

    Node *node  = read_brackets(ptr);
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

static Node * read_brackets(const char **ptr) {
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
    else {
        node = read_number(ptr);
    }
    
    return node;
}

static Node * read_number(const char **ptr) {
    assert(ptr != nullptr);
    assert(*ptr != nullptr);

    assert(isdigit( **ptr));
    double number = **ptr - '0';
    (*ptr)++;
    
    while (isdigit(**ptr)) {
        number *= 10;
        number += **ptr - '0';
        (*ptr)++;
    }

    Node *node = (Node *)calloc(1, sizeof(Node));
    assert(node != nullptr);
    node_init(node);

    node->type         = NUM;
    node->value.number = number;
    
    return node;
}