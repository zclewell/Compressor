#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "glib.h"
#include "freq.h"
#include "tree_node.h"
#include "tree.h"

extern void write_tree(char *tree_name, tree_node *root);
extern tree_node *build_tree(GHashTable *my_dict);

void tree_to_map(tree_node *curr, GHashTable *my_dict, char *path_so_far) {
    if (path_so_far == NULL) {
        path_so_far = malloc(256);
        path_so_far[0] = '\0';
    }
    if (curr->left == NULL && curr->right == NULL) {
        char *path_copy = strdup(path_so_far);
        char *key_copy = malloc(2);
        if (curr->my_freq->character) {
            /* code */
        key_copy[0] = curr->my_freq->character;
        key_copy[1] = '\0';
        g_hash_table_insert(my_dict,key_copy,path_copy);
        fprintf(stderr, "%s has a path of: %s\n",key_copy,path_copy );
        }
    } else {
        size_t len = strlen(path_so_far);
        if (curr->left) {
            char *left_path = strdup(path_so_far);
            char *temp = left_path+len;
            *temp = '0';
            temp[1] = '\0';
            tree_to_map(curr->left, my_dict, left_path);
        }
        if (curr->right) {
            char *right_path = strdup(path_so_far);
            char *temp = right_path+len;
            *temp = '1';
            temp[1] = '\0';
            tree_to_map(curr->right, my_dict, right_path);
        }
    }
    // free(path_so_far);
}

void encode(char* input_file, char *output_file, char *tree_file) {
    fprintf(stderr, "Reading from: %s\n", input_file);
    int input_fd = open(input_file, O_CREAT | O_RDONLY);
    if (input_fd < 0) {
        fprintf(stderr, "%s\n", "ERROR: Could not open input file");
        exit(1);
    }
    GHashTable *my_dict = g_hash_table_new(g_str_hash,g_str_equal);
    char *buf = malloc(2);
    int bytes_read = read(input_fd, buf,1);
    while(bytes_read > 0) {
        buf[1] = '\0';
        if (g_hash_table_contains(my_dict,buf)) {
            ++*(int*)g_hash_table_lookup(my_dict,buf);
        } else {
            int *initial_value = malloc(sizeof(int));
            *initial_value = 1;
            g_hash_table_insert(my_dict,buf,initial_value);
            fprintf(stderr, "Adding %s\n",buf );
        }
        buf = malloc(2);
        bytes_read = read(input_fd, buf,1);
    }
    close(input_fd);
    tree_node *root = build_tree(my_dict);
    while(root->left) {
        fprintf(stderr, "%s\n", "walking...");
        root = root->left;
    }
    // fprintf(stderr, "%c\n", root->my_freq->character);


    // GHashTable *encode_dict = g_hash_table_new(g_str_hash,g_str_equal);
    // // // dictionary encode_dict = char_to_string_dictionary_create();
    // tree_to_map(root,encode_dict,NULL);
    // input_fd = open(input_file, O_CREAT | O_RDONLY);
    // int output_fd = open(output_file, O_CREAT | O_WRONLY);
    // while(read(input_fd,&buf,1)) {
    //     buf[1] = '\0';
    //     // key_value_pair pair = dictionary_at(&buf);
    //     char *curr_string = g_hash_table_lookup(encode_dict,buf);
    //     fprintf(stderr, "%s has a value of: %s\n",buf, curr_string);
    //     if (curr_string) {
    //         write(output_fd, "%s\n", strlen(curr_string));
    //         /* code */
    //     }
    // }
    // close(input_fd);
    // close(output_fd);
    // write_tree(tree_file,root);
}

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Not enough arguments\n");
        exit(1);
    }
    encode(argv[1],argv[2],argv[3]);
    return 0;
}