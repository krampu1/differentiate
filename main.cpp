#include <stdio.h>
#include "main.h"
#include "io/io.h"
#include "tree/tree.h"
#include "differentiate/differentiate.h"
#include "teylor_row/teylor_row.h"

int main() {
    char *buff = nullptr;

    get_data_file(&buff, input_file_name);

    dell_speces(buff);

    Node *tree = buff_to_tree(buff);
    assert(tree != nullptr);

    FILE *file = fopen(TEX_FILE_NAME, "w");
    assert(file != nullptr);

    tex_init(file);

    fprintf(file, "\nрассмотрим такое выражение:\n\n");
    fprintf(file, "$$");
    print_tree_to_tex(file, tree);
    fprintf(file, "$$");

    fprintf(file, "\n\nвычислим его производную:\n\n");
    Node *der = differentiate(tree, file);
    assert(der != nullptr);

    Node *simp = tree_simplification(der);
    assert(simp != nullptr);

    fprintf(file, "упростим:\n\n");
    fprintf(file, "$$");
    print_tree_to_tex(file, der);
    fprintf(file, "=");
    print_tree_to_tex(file, simp);
    fprintf(file, "$$\n\n");

    Node * teylor = teylor_row(tree, 0);

    fprintf(file, "Вычислим разложение по Тейлору:\n\n$$");
    print_tree_to_tex(file, teylor);
    fprintf(file, "$$");

    free(buff);
    del_node(tree);
    tex_del(file);
}