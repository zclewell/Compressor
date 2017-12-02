#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "tree_node.h"
#include "freq.h"
#include "tree.h"

tree_node *root = NULL;

tree_node *remove_smallest(GQueue *a, GQueue *b) {
	tree_node *a_node = g_queue_peek_head(a);
	tree_node *b_node = g_queue_peek_head(b);
	if (a_node && b_node) {
		if (a_node->my_freq.count < b_node->my_freq.count) {
			g_queue_pop_head(a);
			return a_node;
		} else {
			g_queue_pop_head(b);
			return b_node;
		}
	} else if (a_node) {
		g_queue_pop_head(a);
		return a_node;
	} else if (b_node) {
		g_queue_pop_head(b);
		return b_node;
	} 
	return NULL;
}


tree_node *build_tree(GHashTable *my_dict) {
	GList *keys = g_hash_table_get_keys(my_dict);
	GList *values = g_hash_table_get_values(my_dict);

	GQueue *single_queue = g_queue_new();
	GQueue *merge_queue = g_queue_new();
	size_t length = g_list_length(keys);
	for (size_t i = 0; i < length; ++i) {
		tree_node *new_node = malloc(sizeof(tree_node));
		new_node->left = NULL;
		new_node->right = NULL;
		new_node->my_freq.character = *(char*)keys->data;
		new_node->my_freq.count = *(int*)values->data;
	 	g_queue_push_tail(single_queue,new_node);
		keys = keys->next;
		values = values->next;
	 } 
	 while( g_queue_get_length(single_queue) + g_queue_get_length(merge_queue) > 1 ) {
	 	tree_node *merged = malloc(sizeof(tree_node));
	 	merged->left = remove_smallest(single_queue,merge_queue);
	 	if (g_queue_get_length(merge_queue) + g_queue_get_length(single_queue)) {
	 		merged->right = remove_smallest(single_queue,merge_queue);
	 	} else {
	 		merged->right = NULL;
	 	}
	 	merged->my_freq.count = 0;
	 	merged->my_freq.character = '\0';
	 	if (merged->left) {
	 		merged->my_freq.count += merged->left->my_freq.count; 
	 	}
	 	if (merged->right) {
	 		merged->my_freq.count += merged->right->my_freq.count;
	 	}
	 	g_queue_push_tail(merge_queue,merged);
	 }
	 tree_node *output = g_queue_pop_head(merge_queue);
	 return output;
}

void write_tree_recursive(int fd, tree_node *curr) {
	if (curr->left == NULL && curr->right == NULL) {
		char buf[2];
		sprintf(buf,"1%c\n", curr->my_freq.character);
		write(fd,buf,2);
	} else {
		write(fd,"0",1);
		if (curr->left) {
			write_tree_recursive(fd, curr->left);
		}
		if (curr->right) {
			write_tree_recursive(fd, curr->right);
		}
	}

}

void write_tree(char *tree_name, tree_node *root) {
	 int tree_file_fd = open(tree_name, O_CREAT | O_RDWR, 0644);
	 write_tree_recursive(tree_file_fd, root);
	 close(tree_file_fd);
}

tree_node *read_tree_recursive(int fd) {
	char *next_char = malloc(sizeof(char));
	tree_node *new_node = malloc(sizeof(tree_node));
	new_node->left = NULL;
	new_node->right = NULL;
	while(1) {
		int val = read(fd, next_char,1);
		if (val > 0) {
			if (*next_char == '1') {
				read(fd, next_char, 1);
				new_node->my_freq.character = *next_char;
				return new_node;
			} else {
				new_node->left = read_tree_recursive(fd);
				new_node->right = read_tree_recursive(fd);
				return new_node;
			}
		} else if (val < 0) {
			fprintf(stderr, "Error reading from tree file errno: %d\n", errno);
			exit(1);
		} else {
			free(new_node);
			return NULL;
		}
	} 
}

tree_node *read_tree(char *tree_name) {
	int tree_file_fd = open(tree_name, O_CREAT | O_RDONLY);
	if (tree_file_fd == -1) {
		fprintf(stderr, "Error creating: %s errno: %d\n", tree_name,errno);
		exit(1);
	}
	tree_node *root = read_tree_recursive(tree_file_fd);
	close(tree_file_fd);
	return root;
}