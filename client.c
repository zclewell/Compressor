#include <stddef.h>
#include <sys/types.h>

#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

void check_ok(int sockfd) {
    fprintf(stderr, "processing response\n");
    char buf[4];
    read(sockfd,buf,3);
    buf[3]= '\0';
    if (strcmp(buf,"OK\n") == 0) {
        // fprintf(stderr, "%s\n", OK_MESSAGE);
        return;
    } else {
        shutdown(sockfd,SHUT_RD);
        exit(1);
    }
}

void read_complete(int sockfd, void *buf, size_t len) {
    int read_so_far = 0;
    while(read_so_far < len) {
        int ret = read(sockfd, buf + read_so_far, len - read_so_far);
        if (ret > 0) {
            read_so_far += ret;
        } else {
            break;
        }
    }
}

void read_socket_write_file(int sockfd, int fd, size_t len) {
    char buf[1025];
    size_t read_so_far = 0;
    while(read_so_far < len) {
        int ret = read(sockfd,buf, 10);
        // fprintf(stderr, "Read %d\n", ret);
        if (ret > 0) {
            int wrote_so_far = 0;
            while (wrote_so_far < ret) {
                int rett = write(fd, buf + wrote_so_far, ret - wrote_so_far);
                // fprintf(stderr, "Wrote %d\n", rett);
                if (rett) {
                    wrote_so_far += rett;
                }
            }
            read_so_far += ret;
        } else {
            break;
        }
    }
}

size_t get_response_length(int sockfd) {
    size_t len;
    read_complete(sockfd, &len, sizeof(size_t));
    return len;
}

void send_response_length(int sockfd, size_t len) {
    write(sockfd, &len, sizeof(size_t));
}

size_t get_file_length(char *file_name) {
    FILE *file = fopen(file_name, "r");
    fseek(file, 0L, SEEK_END);
    size_t sz = ftell(file);
    fclose(file);
    return sz;
    
}

//Takes a host and a port as strings and return a file descriptor for them (or crashes and burns)
int connect_helper(char *host, char *port) {
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror(NULL);
        exit(1);
    }

    struct addrinfo* res;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int temp;
    if((temp = getaddrinfo(host, port, &hints, &res))) {
        fprintf(stderr, "%s\n", gai_strerror(temp));
        exit(1);
    }

    if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        exit(1);
    }
    
    freeaddrinfo(res);
    return sockfd;
 }


int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: <host> <port> <input> \n");
        exit(1);
    }

    int sockfd = connect_helper(argv[1],argv[2]);
    int input_fd = open(argv[3], O_RDWR);
    send_response_length(sockfd, get_file_length(argv[3]));
    char buf[1024];
    size_t sent_bytes = 0;
    while(1) {
        size_t local_bytes = read(input_fd,buf,1024);
        if (local_bytes > 0) {
            write(sockfd,buf,local_bytes);
        } else {
            break;
        }
    }
    // shutdown(sockfd,SHUT_WR);

    check_ok(sockfd);
    size_t statistics_file_size = get_response_length(sockfd);
    fprintf(stderr, "Expecting stat file of size: %zu\n", statistics_file_size);
    size_t encoded_file_size = get_response_length(sockfd);    
    fprintf(stderr, "Expecting encoded file of size: %zu\n", encoded_file_size);

    size_t extra_file_size = get_response_length(sockfd);
    fprintf(stderr, "Expecting extra file of size: %zu\n", extra_file_size);
        
    int stat_fd = open("stats.txt", O_CREAT | O_RDWR, 0644);
    // fprintf(stderr, "%s\n", "opened stats.txt");
    read_socket_write_file(sockfd, stat_fd, statistics_file_size);
    // fprintf(stderr, "%s\n", "wrote to stats.txt");
    close(stat_fd);
    fprintf(stderr, "%s\n", "Finished reading stats file from server");

    int encoded_fd = open("encoded.txt", O_CREAT | O_RDWR, 0644);
    // fprintf(stderr, "%s\n", "opened encoded.txt");
    read_socket_write_file(sockfd, encoded_fd, encoded_file_size);
    // fprintf(stderr, "%s\n", "wrote to encoded.txt");
    close(encoded_fd);
    fprintf(stderr, "%s\n", "Finished reading encode file from server");


    if (extra_file_size > 0) {
        int extra_fd = open("extra.txt", O_CREAT | O_RDWR, 0644);
        // fprintf(stderr, "%s\n", "opened extra.txt");
        read_socket_write_file(sockfd, extra_fd, extra_file_size);
        // fprintf(stderr, "%s\n", "wrote to extra.txt");
        close(extra_fd);
        fprintf(stderr, "%s\n", "Finished reading extra file from server");
    } 
}