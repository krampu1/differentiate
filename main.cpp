#include <stdio.h>
#include "main.h"
#include "io/io.h"
#include "tree/tree.h"

int main() {
    char *buff = nullptr;

    get_data_file(&buff, input_file_name);

    dell_speces(buff);

    printf("%s", buff);

    Node *tree = buff_to_tree(buff);
    assert(tree != nullptr);

    printf("=%f", calc(tree));

    free(buff);
    del_node(tree);
}