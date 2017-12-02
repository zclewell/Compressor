#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "glib.h"
#include "freq.h"
#include "tree_node.h"
#include "tree.h"

/*
    Takes path to a compressed file and path to a file representing the huffman tree that was used to compress it
    Prints out original file to given path
*/
void decode(char *input_file, char *output_file, char *tree_file) {
    tree_node *root = read_tree(tree_file);
    printf("%s\n", "Generated tree...");
    tree_node *temp = root;
    char buf;
    int input_fd = open(input_file, O_CREAT | O_RDWR);
    int output_fd = open(output_file, O_CREAT | O_RDWR, 0644);

    while(1) {
        char buf;
        int ret_val = read(input_fd, &buf, 1);
        fprintf(stderr, "%c", buf);
        if (ret_val > 0) {
            if (buf == '1') {
                temp = temp->right;
            } else {
                temp = temp->left;
            }
            if (temp->right == NULL && temp->left == NULL) {
                fprintf(stderr, ": %c\n", temp->my_freq.character);
                write(output_fd,&temp->my_freq.character,1);
                temp = root;
            } 
        } else {
            break;
        }
    }
    close(input_fd);
    close(output_fd);
    tree_delete(root);
}

//function to check arguments
int main(int argc, char const *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Not enough arguments\n");
        exit(1);
    }
    decode(argv[1],argv[2],argv[3]);
    return 0;
}