#include "freq.h"
#include "tree_node.h"

void decode(char *input_file, char *output_file, char *tree_file) {
    tree_node *root = read_tree(tree_file);
    tree_node *temp = root;
    char buf;
    int input_fd = open(input_file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
    int output_fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);

    while(1) {
        char buf;
        int ret_val = read(input_fd, &buf, 1);
        if (ret_val) {
            if (buf == '1') {
                temp = temp->right;
            } else {
                temp = temp->left;
            }
            if (temp->right == NULL && temp->left == NULL) {
                fprintf(output_fd, "%c\n",temp->my_freq->character);
                temp = root;
            } 
        } else {
            break;
        }
    }
    close(input_fd);
    close(output_fd);

}