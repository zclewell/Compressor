#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TEMP_FILE "TEMP_FILE"
#define NUM_ALGORITHMS 3
#define THREAD_COUNT 8

#define HUFFMAN_ENCODE_EXE "./huffman/encode"
#define RUN_LENGTH_EXE "./run_length_encoding/encode"
#define L7W_EXE "./l7w/encode"

void send_ok(int sockfd) {
    write(sockfd,"OK\n",3);
}

void read_complete(int sockfd, void *buf, size_t len) {
    int read_so_far = 0;
    while(read_so_far < len) {
        int ret = read(sockfd, buf + read_so_far, len - read_so_far);
        if (ret > 0) {
            read_so_far += ret;
        }
    }
}

void read_socket_write_file(int sockfd, int fd, size_t len) {
    char buf[1025];
    size_t read_so_far = 0;
    while(read_so_far < len) {
        int ret = read(sockfd,buf + read_so_far, 1024);
        if (ret > 0) {
            buf[ret] = '\0';
            dprintf(fd,"%s",buf);
            read_so_far += ret;
        }
    }
}

typedef struct worker_struct {
    char *file_name;
} worker_struct;

typedef struct stat_struct {
    size_t original_file_size;
    size_t compressed_file_size;
} stat_struct;

typedef struct return_struct {
    stat_struct *my_stats;
    char *encoded_file_name;
    char *extra_file_name;
} return_struct;

size_t get_file_length(char *file_name) {
    FILE *file = fopen(file_name, "r");
    fseek(file, 0L, SEEK_END);
    size_t sz = ftell(file);
    fclose(file);
    return sz;
    
}

void huffman_worker(void *data) {
    worker_struct *my_worker_struct = data;
    char *encoded_file_name = "huffman_encoded.txt";
    char *tree_file_name = "huffman_tree.txt";

    pid_t child  = fork();
    if (child) {
        int status;
        waitpid(child,&status);
        if (status) {
            exit(1);
        }
    } else {
        execl(HUFFMAN_ENCODE_EXE,"encode",my_worker_struct->file_name,encoded_file_name,tree_file_name,NULL);
        fprintf(stderr, "exec failed for huffman\n");
        exit(1);
    }

    stat_struct *my_stat_struct = malloc(sizeof(stat_struct));
    my_stat_struct->original_file_size = get_file_length(my_worker_struct->file_name);
    my_stat_struct->compressed_file_size = get_file_length(encoded_file_name)/8 + get_file_length(tree_file_name);

    return_struct my_return_struct = malloc(sizeof(return_struct));
    my_return_struct->my_stats = my_stat_struct;
    my_return_struct->encoded_file_name = encoded_file_name;
    my_return_struct->extra_file_name = tree_file_name;

    return my_return_struct;
}

void run_length_worker(void *data) {


    stat_struct *my_stat_struct = malloc(sizeof(stat_struct));
    my_stat_struct->original_file_size = 0;
    my_stat_struct->compressed_file_size = 0;

    return_struct my_return_struct = malloc(sizeof(return_struct));
    my_return_struct->my_stats = my_stat_struct;
    my_return_struct->encoded_file_name = encoded_file_name;
    my_return_struct->extra_file_name = NULL;

    return my_return_struct;

}

void l7w_worker(void *data) {


    stat_struct *my_stat_struct = malloc(sizeof(stat_struct));
    my_stat_struct->original_file_size = 0;
    my_stat_struct->compressed_file_size = 0;

    return_struct my_return_struct = malloc(sizeof(return_struct));
    my_return_struct->my_stats = my_stat_struct;
    my_return_struct->encoded_file_name = encoded_file_name;
    my_return_struct->extra_file_name = NULL;

    return my_return_struct;
}

size_t get_response_length(int sockfd) {
    size_t len;
    read_complete(sockfd, &len, sizeof(len));
    return len;
}

void accept_connections(struct epoll_event *e,int epoll_fd) {
    while(1)
    {
        struct sockaddr_in new_addr;
        socklen_t new_len = sizeof(new_addr);
        int new_fd = accept(e->data.fd, (struct sockaddr*) &new_addr, &new_len);

        if(new_fd == -1) {
            // All pending connections handled
            if(errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
            {
                perror("accept");
                exit(1);
            }
        }

        char *connected_ip= inet_ntoa(new_addr.sin_addr);
        int port = ntohs(new_addr.sin_port);
        printf("Accepted Connection %s port %d\n", connected_ip, port);

        int flags = fcntl(new_fd, F_GETFL, 0);
        fcntl(new_fd, F_SETFL, flags | O_NONBLOCK);

        //Connection to epoll
        struct epoll_event event;
        event.data.fd = new_fd;
        event.events = EPOLLIN | EPOLLET;
        if(epoll_ctl (epoll_fd, EPOLL_CTL_ADD, new_fd, &event) == -1)
        {
            perror("accept epoll_ctl");
            exit(1);
        }
    }
}

void handle_data(struct epoll_event *e) {
    int sockfd = e->data.fd;
    char first_char[1];
    send_ok(sockfd);
    size_t len = get_response_length(sockfd);
    int temp_fd = open(TEMP_FILE, O_CREAT | O_RDWR);
    read_socket_write_file(sockfd, temp_fd, len);

    pthread_t threads[3];
    worker_struct *my_worker_struct = malloc(size_of(worker_struct));

    my_worker_struct->file_name = TEMP_FILE;
    pthread_create(threads, NULL, huffman_worker, my_worker_struct);
    pthread_create(threads + 1, NULL, run_length_worker, my_worker_struct);
    pthread_create(threads + 2, NULL, l7w_worker, my_worker_struct);

    return_struct return_struct_arr[NUM_ALGORITHMS];
    for (int i = 0; i < NUM_ALGORITHMS; ++i) {
        pthread_join(threads[i], &return_struct_arr[i]);
    }




}

int main(int argc, char** argv) {
    if( argc != 2) {
        fprintf(stderr, "./server <port>\n");
        exit(1);
    }

    fprintf(stderr, "Initializing server\n");
    int s;
    // struct epoll_event *events;

    // Create the socket as a nonblocking socket
    int sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    struct addrinfo hints, *result = NULL;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, argv[1], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }

    int optval = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if ( bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0 ){
        perror("bind()");
        exit(1);
    }

    if ( listen(sock_fd, 10) != 0 ) {
        perror("listen()");
        exit(1);
    }

    struct sockaddr_in sin;
    socklen_t socklen = sizeof(sin);
    if (getsockname(sock_fd, (struct sockaddr *)&sin, &socklen) == -1) {
        perror("getsockname");
    } else {
        printf("Listening on port number %d\n", ntohs(sin.sin_port));
    }

    //setup epoll
    int epoll_fd = epoll_create(1);
    if(epoll_fd == -1) {
        perror("epoll_create()");
        exit(1);
    }

    struct epoll_event event;
    event.data.fd = sock_fd;
    event.events = EPOLLIN | EPOLLET;

    //Add the sever socket to the epoll
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event)) {
        perror("epoll_ctl()");
        exit(1);
    }


    // Event loop
    while(1) {
        struct epoll_event new_event;

        if(epoll_wait(epoll_fd, &new_event, 1, -1) > 0) {
            //Probably check for errors

            // New Connection Ready
            if(sock_fd == new_event.data.fd) {
                accept_connections(&new_event, epoll_fd);
            } else {
                handle_data(&new_event);
            }
        }
    }
    return 0;
}