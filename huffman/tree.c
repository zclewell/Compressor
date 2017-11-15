#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "tree_node.h"
#include "freq.h"
#include "tree.h"

tree_node *root = NULL;

tree_node *remove_smallest(GQueue *a, GQueue *b) {
	size_t a_len = g_queue_get_length(a);
	size_t b_len = g_queue_get_length(b);
	fprintf(stderr, "a_len: %zu b_len: %zu\n",a_len,b_len );
	if (a_len && b_len) {
		tree_node *a_node = g_queue_peek_head(a);
		tree_node *b_node = g_queue_peek_head(b);
		freq_t *a_freq = a_node->my_freq;
		freq_t *b_freq = b_node->my_freq;
		if (a_freq->count < b_freq->count) {
			a_node = g_queue_pop_head(a);
			return a_node;
		} else {
			b_node = g_queue_pop_head(b);
			return b_node;
		}
	} else if (a_len) {
		tree_node *a_node = g_queue_pop_head(a);
		return a_node;
	} else if (b_len) {
		tree_node *b_node = g_queue_pop_head(b);
		return b_node;
	} else {
		return NULL;
	}
}


tree_node *build_tree(GHashTable *my_dict) {
	GList *keys = g_hash_table_get_keys(my_dict);
	GList *values = g_hash_table_get_values(my_dict);

	GQueue *single_queue = g_queue_new();
	GQueue *merge_queue = g_queue_new();
	size_t length = g_list_length(keys);
	for (size_t i = 0; i < length; ++i) {
		char *curr = keys->data;
		int *curr_val = values->data;
		// fprintf(stderr, "Currently on: %c:%d\n", *curr, *curr_val);
		keys = keys->next;
		values = values->next;
		tree_node *new_node = malloc(sizeof(tree_node));
		new_node->my_freq = malloc(sizeof(freq_t));
		new_node->my_freq->character = *curr;
		new_node->my_freq->count = *curr_val;
		fprintf(stderr, "New Node: %s,%d\n",curr,*curr_val );
	 	g_queue_push_tail(single_queue,new_node);
	 } 
	 // g_queue_push_tail(merge_queue,g_queue_pop_head(single_queue));
	 while( g_queue_get_length(single_queue) + g_queue_get_length(merge_queue) > 1 ) {
	 	tree_node *new_left = remove_smallest(single_queue,merge_queue);
	 	tree_node *new_right = NULL;
	 	if (g_queue_get_length(merge_queue) + g_queue_get_length(single_queue)) {
	 		new_right = remove_smallest(single_queue,merge_queue);
	 	}
	 	tree_node *merged = malloc(sizeof(tree_node));
	 	merged->my_freq = malloc(sizeof(freq_t));
	 	if (new_left) {
	 		merged->my_freq->count += new_left->my_freq->count; 
	 	}
	 	if (new_right) {
	 		merged->my_freq->count += new_right->my_freq->count;
	 	}
	 	merged->left = new_left;
	 	merged->right = new_right;
	 	g_queue_push_tail(merge_queue,merged);
	 }
	 tree_node *output = g_queue_pop_head(merge_queue);
	 return output;
}

void write_tree_recursive(int fd, tree_node *curr) {
	if (curr->left == NULL && curr->right == NULL) {
		fprintf(fd, "%d%c", 1, curr->my_freq->character);
	} else {
		fprintf(fd, "%d", 0);
		write_tree_recursive(fd, curr->left);
		write_tree_recursive(fd, curr->right);
	}

}

void write_tree(char *tree_name, tree_node *root) {
	 int tree_file_fd = open(tree_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
	 write_tree_recursive(tree_file_fd, root);
	 close(tree_file_fd);
}

tree_node *read_tree_recursive(int fd) {
	char next_char;
	tree_node *new_node = malloc(sizeof(tree_node));
	if(read(fd, &next_char, 1)) {
		if (next_char == '1') {
			new_node->my_freq = malloc(sizeof(freq_t));
			read(fd, &next_char, 1);
			new_node->my_freq->character = next_char;
			return new_node;
		} else {
			new_node->left = read_tree_recursive(fd);
			new_node->right = read_tree_recursive(fd);
			return new_node;
		}
	} else {
		return NULL;
	}

}
tree_node *read_tree(char *tree_name) {
	int tree_file_fd = open(tree_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
	tree_node *root = read_tree_recursive(tree_file_fd);
	close(tree_file_fd);
	return root;
}