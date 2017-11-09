
void tree_to_map(tree_node *curr, dictionary *my_dict, char *path_so_far) {
    if (path_so_far == NULL) {
        path_so_far = malloc(256);
        path_so_far[0] = '\0';
    }
    if (curr->left == NULL && curr->right == NULL) {
        dictionary_set(my_dict,curr->my_freq->character,path_so_far);
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
    int input_fd = open(input_file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
    dictionary *my_dict = char_to_int_dictionary_create();
    char buf;
    while(read(input_fd,&buf,1)) {
        key_value_pair old = dictionary_at(my_dict,&buf);
        if (old.value) {
            int new_count = key_value_pair.value[0][0] + 1;
            dictionary_set(my_dict, &buf, new_count);
        } else {
            int *value = malloc(sizeof(int));
            *value = 1;
            dictionary_set(my_dict, &buf, value);
        }
    }
    close(input_fd);
    tree_node *root = build_tree(my_dict);
    dictionary encode_dict = char_to_string_dictionary_create();
    tree_to_map(root,encode_dict,NULL);
    input_fd = open(input_file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
    int output_fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
    while(read(input_fd,&buf,1)) {
        key_value_pair pair = dictionary_at(&buf);
        fprintf(output_fd, "%s\n", pair.value);
    }
    close(input_fd);
    close(output_fd);
    write_tree(tree_file,tree_file);
}
