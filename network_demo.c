/*
** showip.c -- show IP addresses for a host given on the command line
*/

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Incorrect usage... exiting\n");
        exit(0);
    } 
    char *address = strdup(argv[1]);
    int fd = get_non_local_fd(address);
    fprintf(stderr, "Fd for: %s opened at: %d\n", address,fd);
}

int get_non_local_fd(char* address) {
    pid_t child = fork();
    int debug_file = open("debug.info", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
    if (child) {
        dup2(debug_file,2); //send info from wget into a debug file so we don't have to see it
        execlp("wget","wget","-O","download.tmp",address,NULL);
        fprintf(stderr, "Exec failed... exiting");
        exit(1);
    }
    waitpid(child,0,0);
    int download_fd = open("download.tmp",  O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | O_CLOEXEC);
    return download_fd;
}



