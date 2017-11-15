#pragma once

#include "freq.h"

typedef struct tree_node {
    freq_t my_freq;
	struct tree_node* left;
	struct tree_node* right;
} tree_node;

