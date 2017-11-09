#define DEBUG_FILE "debug.info"

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int get_non_local_fd(char *address, char *temp_name) {
    pid_t child = fork();
    int debug_file = open(DEBUG_FILE, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
    if (child) {
        dup2(debug_file,2); //send info from wget into a debug file so we don't have to see it
        execlp("wget","wget","-O",temp_name,address,NULL);
        fprintf(stderr, "Exec failed... exiting");
        exit(1);
    }
    waitpid(child,0,0);
    int download_fd = open(temp_name,  O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
    return download_fd;
}