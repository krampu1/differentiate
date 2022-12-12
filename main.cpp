#include <stdio.h>
#include "main.h"
#include "io/io.h"
#include "tree/tree.h"
#include "differentiate/differentiate.h"

int main() {
    char *buff = nullptr;

    get_data_file(&buff, input_file_name);

    dell_speces(buff);

    fprintf(stderr, "%s", buff);

    Node *tree = buff_to_tree(buff);
    assert(tree != nullptr);

    //printf("=%f", calc(tree));
    
    printf("=");
    print_tree(tree);
    printf("\n");

    FILE *file = fopen(TEX_FILE_NAME, "w");
    assert(file != nullptr);

    tex_init(file);

    fprintf(file, "\nрассмотрим такое выражение:\n\n");
    print_tree_to_tex(file, tree);

    fprintf(file, "\n\nвычислим его производную:\n\n");
    Node *der = diffirentiate(tree, file);
    print_tree(der);
    printf("\n");

    fprintf(file, "вычислим его производную:\n\n");
    fprintf(file, "(");
    print_tree_to_tex(file, tree);
    fprintf(file, ")\'=");    
    print_tree_to_tex(file, der);

    free(buff);
    del_node(tree);
    tex_del(file);
}