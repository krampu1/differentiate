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

    Node *diff = diffirentiate(tree);

    print_tree(diff);
    printf("\n");

    free(buff);
    del_node(tree);
}