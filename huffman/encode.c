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
        g_hash_table_insert(my_dict,curr->my_freq->character,path_so_far);
        // dictionary_set(my_dict,curr->my_freq->character,path_so_far);
    } else {
        size_t len = strlen(path_so_far);
        char *left_path = strdup(path_so_far);
        char *temp = left_path+len;
        *temp = '0';
        temp[1] = '\0';
        tree_to_map(curr->left, my_dict, left_path);
        char *right_path = strdup(path_so_far);
        temp = right_path+len;
        *temp = '1';
        temp[1] = '\0';
        tree_to_map(curr->right, my_dict, right_path);
    }
    free(path_so_far);
}

void encode(char* input_file, char *output_file, char *tree_file) {
    fprintf(stderr, "Reading from: %s\n", input_file);
    int input_fd = open(input_file, O_CREAT | O_WRONLY);
    if (input_fd < 0) {
        fprintf(stderr, "%s\n", "ERROR: Could not open input file");
        exit(1);
    }
    // dictionary *my_dict = char_to_int_dictionary_create();
    GHashTable *my_dict = g_hash_table_new(g_str_hash,g_str_equal);
    char *buf = malloc(1);
    int bytes_read = read(input_fd, &buf,1);
    fprintf(stderr, "Read: %d bytes from: %d\n",bytes_read,input_fd );
    while(bytes_read > 0) {
        // if (g_hash_table_contains(my_dict,buf)) {
        //     int old_value = *(int*)(g_hash_table_lookup(my_dict,buf)) + 1;
        //     g_hash_table_insert(my_dict,buf,&old_value);
        // } else {
        //     int initial_value = 1;
        //     g_hash_table_insert(my_dict,buf,&initial_value);
        // }
        bytes_read = read(input_fd, buf,1);
    }
    close(input_fd);
    // tree_node *root = build_tree(my_dict);
    // GHashTable *encode_dict = g_hash_table_new(g_str_hash,g_str_equal);
    // // dictionary encode_dict = char_to_string_dictionary_create();
    // tree_to_map(root,encode_dict,NULL);
    // input_fd = open(input_file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
    // int output_fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
    // while(read(input_fd,&buf,1)) {
    //     // key_value_pair pair = dictionary_at(&buf);
    //     char *curr_string = g_hash_table_lookup(encode_dict,&buf);
    //     fprintf(output_fd, "%s\n", curr_string);
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