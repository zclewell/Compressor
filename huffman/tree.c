#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "tree_node.h"
#include "freq.h"
#include "tree.h"

tree_node *root = NULL;

freq_t *remove_smallest(GQueue *a, GQueue *b) {
	// size_t a_len = queue_size(a);
	// size_t b_len = queue_size(b);
	size_t a_len = g_queue_get_length(a);
	size_t b_len = g_queue_get_length(b);
	if (a_len && b_len) {
		// tree_node *a_node = queue_front(a);
		tree_node *a_node = g_queue_peek_head(a);
		// tree_node *b_node = queue_front(b);
		tree_node *b_node = g_queue_peek_head(b);
		freq_t *a_freq = a_node->my_freq;
		freq_t *b_freq = b_node->my_freq;
		if (a_freq->count < b_freq->count) {
			// free(queue_pull(a));
			free(g_queue_pop_head(a));
			return a_freq;
		} else {
			// free(queue_pull(b));
			free(g_queue_pop_head(b));
			return b_freq;
		}
	} else if (a_len) {
		// tree_node *small_node = queue_pull(a);
		tree_node *small_node = g_queue_pop_head(a);
		freq_t *return_freq = small_node->my_freq;
		free(small_node);
		return return_freq;
	} else {
		// tree_node *small_node = queue_pull(b);
		tree_node *small_node = g_queue_pop_head(b);
		freq_t *return_freq = small_node->my_freq;
		free(small_node);
		return return_freq;
	}
}


tree_node *build_tree(GHashTable *my_dict) {
	// vector *keys = dictionary_keys(my_dict);
	GList *keys = g_hash_table_get_keys(my_dict);

	// queue *single_queue = queue_create(-1);
	GQueue *single_queue = g_queue_new();
	// queue *merge_queue = queue_create(-1);
	GQueue *merge_queue = g_queue_new();
	size_t length = g_list_length(keys);
	for (size_t i = 0; i < length; ++i) {
		char *curr = keys->data;
		keys = keys->next;
		tree_node *new_node = malloc(sizeof(tree_node));
		new_node->my_freq = malloc(sizeof(freq_t));
		new_node->my_freq->character = *curr;
		// new_node->my_freq->count = *dictionary_get(my_dict, curr);
		new_node->my_freq->count = g_hash_table_lookup(my_dict,curr);
	 	// queue_push(single_queue, new_node);
	 	g_queue_push_tail(single_queue,new_node);
	 } 
	 while( g_queue_get_length(single_queue) + g_queue_get_length(merge_queue) > 1 ) {
	 	tree_node *new_left = malloc(sizeof(tree_node));
	 	tree_node *new_right = NULL;
	 	new_left->my_freq = remove_smallest(single_queue,merge_queue);
	 	if (g_queue_get_length(merge_queue) + g_queue_get_length(single_queue)) {
	 		new_right = malloc(sizeof(tree_node));
	 		new_right->my_freq = remove_smallest(single_queue,merge_queue);
	 	}
	 	tree_node *merged = malloc(sizeof(tree_node));
	 	merged->my_freq = malloc(sizeof(freq_t));
	 	merged->my_freq->count = new_left->my_freq->count + new_right->my_freq->count;
	 	merged->left = new_left;
	 	merged->right = new_right;
	 	// queue_push(merge_queue,merged);
	 	g_queue_push_tail(merge_queue,merged);
	 }
	 // tree_node *output = queue_pull(merge_queue);
	 tree_node *output = g_queue_pop_head(merge_queue);
	 // queue_destroy(single_queue);
	 // queue_destroy(merge_queue);
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