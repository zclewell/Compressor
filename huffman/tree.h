#include "tree_node.h"
#include "freq.h"
#include "glib.h"

//helper function needed to create tree from a dict
freq_t *remove_smallest(GQueue *a, GQueue *b);

//build tree from a dict of character-frequency key-value pairings
tree_node *build_tree(GHashTable *my_dict);

//write a tree to a file for later use
void write_tree(char *tree_name, tree_node *root);

//helper functon for write_tree
void write_tree_recursive(int fd, tree_node *curr);

//generate a tree from a file 
tree_node *read_tree(char *tree_name);

//helper fuction for read_tree
tree_node *read_tree_recursive(int fd);
