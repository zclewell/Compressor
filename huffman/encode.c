#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "glib.h"
#include "freq.h"
#include "tree_node.h"
#include "tree.h"

extern void write_tree(char *tree_name, tree_node *root);
extern tree_node *build_tree(GHashTable *my_dict);

typedef struct worker_struct {
    size_t start_byte;
    size_t end_byte;
    char *input_file;
} worker_struct;

void *worker(void *data) {
    worker_struct *my_struct = data;
    GHashTable *my_dict = g_hash_table_new(g_str_hash,g_str_equal);
    int input_fd = open(my_struct->input_file, O_RDWR);
    lseek(input_fd,my_struct->start_byte,SEEK_SET);
    char *buf = malloc(2);
    for (size_t i = my_struct->start_byte; i < my_struct->end_byte; ++i) {
        if (read(input_fd,buf,1)) {
            buf[1] = '\0';
            if (g_hash_table_contains(my_dict,buf)) {
                ++*(int*)g_hash_table_lookup(my_dict,buf);
            } else {
                int *initial_value = malloc(sizeof(int));
                *initial_value = 1;
                g_hash_table_insert(my_dict,buf,initial_value);
            }
            buf = malloc(2);
        } else {
            break;
        }
    }
    close(input_fd);
    return my_dict;
}

void tree_to_map(tree_node *curr, GHashTable *my_dict, char *path_so_far) {
    if (path_so_far == NULL) {
        path_so_far = malloc(256);
        path_so_far[0] = '\0';
    }
    if (curr->left == NULL && curr->right == NULL) {
        char *path_copy = strdup(path_so_far);
        char *key_copy = malloc(2);
        if (curr->my_freq.character) {
            key_copy[0] = curr->my_freq.character;
            key_copy[1] = '\0';
            g_hash_table_insert(my_dict,key_copy,path_copy);
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
    free(path_so_far);
}

void encode(char* input_file, char *output_file, char *tree_file, int thread_count) {
    fprintf(stderr, "Reading from: %s\n", input_file);
    size_t file_size = 0;
    FILE *temp_file = fopen(input_file, "r");
    if (temp_file) {
        fseek(temp_file, 0L, SEEK_END);
        file_size = ftell(temp_file);
    } else {
        fprintf(stderr, "Failed to open: %s\n", input_file);
        exit(1);
    }

    pthread_t threads[thread_count];
    size_t interval_size = file_size/thread_count;
    for (int i = 0; i < thread_count; ++i) {
        worker_struct *temp_struct = malloc(sizeof(worker_struct));
        temp_struct->start_byte = interval_size*i;
        if (i == thread_count - 1) {
            temp_struct->end_byte = file_size;
        } else {
            temp_struct->end_byte = interval_size*(i+1);
        }
        temp_struct->input_file = input_file;
        fprintf(stderr, "Thread: %d reading from: %zu to: %zu\n",i,temp_struct->start_byte,temp_struct->end_byte);
        pthread_create(threads+i,NULL,worker,temp_struct);
    }

    GHashTable *dict_arr[thread_count];
    int failed = 0;
    for (int i = 0; i < thread_count; ++i) {
        if (pthread_join(threads[i],dict_arr+i)) {
            failed = 1;
        }
    }
    if (failed) {
        fprintf(stderr, "One of the worker threads failed... exiting\n");
        exit(1);
    }

    GHashTable *my_dict = dict_arr[0];
    for (int i = 1; i < thread_count; ++i) {
        GList *keys = g_hash_table_get_keys(dict_arr[i]);
        GList *values = g_hash_table_get_values(dict_arr[i]);

        size_t length = g_list_length(keys);
        for (size_t i = 0; i < length; ++i) {
            if (g_hash_table_contains(my_dict,keys->data)) {
                *(int*)g_hash_table_lookup(my_dict,keys->data) += values->data;
            } else {
                int *initial_value = malloc(sizeof(int));
                *initial_value = values->data;
                g_hash_table_insert(my_dict,keys->data,initial_value);
            }
            keys = keys->next;
            values = values->next;
        }
    }
    tree_node *root = build_tree(my_dict);

    GHashTable *encode_dict = g_hash_table_new(g_str_hash,g_str_equal);
    tree_to_map(root,encode_dict,NULL);
    int input_fd = open(input_file, O_CREAT | O_RDWR);
    fprintf(stderr, "Writing to: %s\n", output_file);
    int output_fd = open(output_file, O_CREAT | O_RDWR, 0644);
    char buf[2];
    while(read(input_fd,buf,1)) {
        buf[1] = '\0';
        char *curr_string = g_hash_table_lookup(encode_dict,buf);
        if (curr_string) {
            write(output_fd, curr_string, strlen(curr_string));
        }
    }
    close(input_fd);
    close(output_fd);
    fprintf(stderr, "Saving tree at: %s\n", tree_file);
    write_tree(tree_file,root);
}

int main(int argc, char const *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Not enough arguments\n");
        exit(1);
    }
    encode(argv[1],argv[2],argv[3],atoi(argv[4]));
    return 0;
}