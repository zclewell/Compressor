#include "tree_node.h"
#include "freq.h"

tree_node *root = NULL;

my_freq *remove_smallest(queue *a, queue *b) {
	size_t a_len = queue_size(a);
	size_t b_len = queue_size(b);
	if (a_len && b_len) {
		tree_node *a_node = queue_front(a);
		tree_node *b_node = queue_front(b);
		freq_t *a_freq = a_node->my_freq;
		freq_t *b_freq = b_node->my_freq;
		if (a_freq->count < b_freq->count) {
			free(queue_pull(a));
			return a_freq;
		} else {
			free(queue_pull(b));
			return b_freq;
		}
	} else if (a_len) {
		tree_node *small_node = queue_pull(a);
		freq_t *return_freq = small_node->my_freq;
		free(small_node);
		return(return_freq);
	} else {
		tree_node *small_node = queue_pull(b);
		freq_t *return_freq = small_node->my_freq;
		free(small_node);
		return(return_freq)
	}
}


tree_node *build_tree(dict *my_dict) {
	vector *keys = dictionary_keys(my_dict);
	queue *single_queue = queue_create(-1);
	queue *merge_queue = queue_create(-1);
	for (size_t i = 0; i < vector_size(keys); ++i) {
		char *curr = vector_get(keys,i);
		tree_node *new_node = malloc(sizeof(tree_node));
		new_node->my_freq = malloc(sizeof(freq_t));
		new_node->my_freq->character = *curr;
		new_node->my_freq->count = *dictionary_get(my_dict, curr);
	 	queue_push(single_queue, new_node);
	 } 
	 while( single_len + merge_len > 1 ) {
	 	tree_node *new_left = malloc(sizeof(tree_node));
	 	tree_node *new_right = NULL;
	 	new_left->my_freq = remove_smallest(single_queue,merge_queue);
	 	if (vector_size(single_queue) + vector_size(single_queue)) {
	 		new_right = malloc(sizeof(tree_node));
	 		new_right->my_freq = remove_smallest(single_queue,merge_queue);
	 	}
	 	tree_node *merged = malloc(sizeof(tree_node));
	 	merged->my_freq = malloc(sizeof(freq_t));
	 	merged->my_freq->count = new_left->my_freq->count + new_right->my_freq->count;
	 	merged->left = new_left;
	 	merged->right = new_right;
	 	queue_push(merge_queue,merged);
	 }
	 tree_node *output = queue_pull(merge_queue);
	 queue_destroy(single_queue);
	 queue_destroy(merge_queue);
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
	read(fd, &next_char, 1);
	if (next_char == '1') {
		new_node->my_freq = malloc(sizeof(freq_t));
		read(fd, &next_char, 1);
		new_node->my_freq->character = next_char;
		return new_node;
	} else {
		new_node->left = write_tree_recursive(fd);
		new_right->right = write_tree_recursive(fd);
		return new_node;
	}
}
tree_node *read_tree(char *tree_name) {
	int tree_file_fd = open(tree_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
	tree_node *root = read_tree_recursive(tree_file_fd);
	close(tree_file_fd);
	return root;
}