/**
 * Chatroom Lab
 * CS 241 - Fall 2017
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_CLIENTS 8

#define TEMP_FILE "TEMP_FILE"
#define NUM_ALGORITHMS 3
#define THREAD_COUNT "8"

#define HUFFMAN_ENCODE_EXE "./huffman/encode"
#define RUN_LENGTH_EXE "./run_length_encoding/rL"
#define L7W_EXE "./l7w/lzw"

#define EXEC_ERR "An error occured when execing... Exiting"

static volatile int endSession;
static volatile int serverSocket;

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
		char *encode_type;
} return_struct;

void close_server() {
		endSession = 1;
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

size_t get_file_length(char *file_name) {
		if (!file_name) {
			return 0;
		}
    FILE *file = fopen(file_name, "r");
    fseek(file, 0L, SEEK_END);
    size_t sz = ftell(file);
    fclose(file);
    return sz;
    
}


void read_fd_write_fd(int readfd, int writefd, size_t len) {
    char buf[1024];
    size_t read_so_far = 0;
    while(read_so_far < len) {
        int ret = read(readfd,buf, 1024);
        // fprintf(stderr, "Read %d\n",  ret);
        if (ret > 0) {
        		int wrote_so_far = 0;
        		while(wrote_so_far < ret) {
        			int rett = write(writefd, buf + wrote_so_far, ret - wrote_so_far);
        			// fprintf(stderr, "wrote %d\n", rett);
        			if (rett > 0) {
        				wrote_so_far += rett;
        			}
        		}
            // buf[ret] = '\0';
            // dprintf(writefd,"%s",buf);
            read_so_far += ret;
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


void cleanup() {
		if (shutdown(serverSocket, SHUT_RDWR) != 0) {
				perror("shutdown():");
		}
		close(serverSocket);
}

void *huffman_worker(void *data) {
		worker_struct *my_worker_struct = data;
		char *encoded_file_name = "huffman_encoded.txt";
		char *tree_file_name = "huffman_tree.txt";

		pid_t child  = fork();
		if (child) {
				int status;
				waitpid(child,&status,0);
				if (status) {
						exit(1);
				}
		} else {
				close(2);
				execl(HUFFMAN_ENCODE_EXE, HUFFMAN_ENCODE_EXE,my_worker_struct->file_name,encoded_file_name,tree_file_name,THREAD_COUNT,NULL);
				fprintf(stderr, "%s\n", EXEC_ERR);
				exit(1);
		}

		stat_struct *my_stat_struct = malloc(sizeof(stat_struct));
		my_stat_struct->original_file_size = get_file_length(my_worker_struct->file_name);
		my_stat_struct->compressed_file_size = get_file_length(encoded_file_name)/8 + get_file_length(tree_file_name);

		return_struct *my_return_struct = malloc(sizeof(return_struct));
		my_return_struct->my_stats = my_stat_struct;
		my_return_struct->encoded_file_name = encoded_file_name;
		my_return_struct->extra_file_name = tree_file_name;
		my_return_struct->encode_type = "huffman";
		fprintf(stderr, "\thuffman thread finished!\n");
		return my_return_struct;
}

void *run_length_worker(void *data) {
		worker_struct *my_worker_struct = data;
		char *encoded_file_name = "run_length_encoded.txt";

		pid_t child = fork();
		if (child) {
				int status;
				waitpid(child,&status,0);
				if (status) {
						exit(1);
				}
		} else {
				execl(RUN_LENGTH_EXE, RUN_LENGTH_EXE, "1", my_worker_struct->file_name, encoded_file_name, NULL);
				fprintf(stderr, "%s\n", EXEC_ERR);
				exit(1);
		}

		stat_struct *my_stat_struct = malloc(sizeof(stat_struct));
		my_stat_struct->original_file_size = get_file_length(my_worker_struct->file_name);
		my_stat_struct->compressed_file_size = get_file_length(encoded_file_name);

		return_struct *my_return_struct = malloc(sizeof(return_struct));
		my_return_struct->my_stats = my_stat_struct;
		my_return_struct->encoded_file_name = encoded_file_name;
		my_return_struct->extra_file_name = NULL;
		my_return_struct->encode_type = "run length";
		fprintf(stderr, "\trun length thread finished!\n");

		return my_return_struct;
}

void *l7w_worker(void *data) {
	worker_struct *my_worker_struct = data;
	char *encoded_file_name = "l7w_encoded.txt";

	pid_t child = fork();
	if (child) {
		int status;
		waitpid(child, &status, 0);
		if (status) {
			exit(1);
		}
	} else {
		execl(L7W_EXE, L7W_EXE, "1", my_worker_struct->file_name, encoded_file_name, NULL);
		fprintf(stderr, "%s\n", EXEC_ERR);
		exit(1);
	}
		stat_struct *my_stat_struct = malloc(sizeof(stat_struct));
		my_stat_struct->original_file_size = get_file_length(my_worker_struct->file_name);
		my_stat_struct->compressed_file_size = get_file_length(encoded_file_name);

		return_struct *my_return_struct = malloc(sizeof(return_struct));
		my_return_struct->my_stats = my_stat_struct;
		my_return_struct->encoded_file_name = encoded_file_name;
		my_return_struct->extra_file_name = NULL;
		my_return_struct->encode_type = "l7w";
		fprintf(stderr, "\tlzw thread finished!\n");
		return my_return_struct;
}

void run_server(char *port) {
		int sockfd;
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
			perror(NULL);
			exit(1);
		}
		int optval = 1;
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int));
		serverSocket = sockfd;

		struct addrinfo hints, *result;
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;
		int s = getaddrinfo(NULL, port, &hints, &result);
		if(s){
			fprintf(stderr, "%s\n", gai_strerror(s));
			exit(1);
		}

		if (bind(sockfd, result->ai_addr, result->ai_addrlen) == -1){
			perror("bind");
			exit(1);
		}

		intptr_t indexes[MAX_CLIENTS];
		while(!endSession){
			if(listen(sockfd, MAX_CLIENTS)){
				perror(NULL);
				exit(1);
			}
			int clientfd = accept(sockfd, NULL, NULL);
			if(clientfd == -1){
				perror(NULL);
				exit(1);
			}

			fprintf(stderr, "[SERVER] received new client\n");

			size_t size_of_file = get_response_length(clientfd);
			int original_fd = open(TEMP_FILE, O_CREAT | O_RDWR, 0644);
			read_fd_write_fd(clientfd, original_fd, size_of_file);

			pthread_t threads[3];
			worker_struct *my_worker_struct = malloc(sizeof(worker_struct));

			fprintf(stderr, "[SERVER] starting worker threads...\n");
			my_worker_struct->file_name = TEMP_FILE;
			pthread_create(threads, NULL, huffman_worker, my_worker_struct);
			pthread_create(threads + 1, NULL, run_length_worker, my_worker_struct);
			pthread_create(threads + 2, NULL, l7w_worker, my_worker_struct);

			return_struct *return_struct_arr[NUM_ALGORITHMS];
			for (int i = 0; i < NUM_ALGORITHMS; ++i) {
				 pthread_join(threads[i], return_struct_arr + i);
			}
			fprintf(stderr, "[SERVER] all worker threads done!\n");

			write(clientfd, "OK\n", 3);
			return_struct *best_return_struct = return_struct_arr[0];
			for (int i = 1; i < NUM_ALGORITHMS; ++i) {
				if (return_struct_arr[i]->my_stats->compressed_file_size < best_return_struct->my_stats->compressed_file_size) {
					best_return_struct = return_struct_arr[i];
				}
			}

			int stats_fd = open("server_stats.txt", O_CREAT | O_RDWR, 0644);
			dprintf(stats_fd,"%s\nOriginal filesize: %zu\nCompressed filesize: %zu\n",best_return_struct->encode_type,best_return_struct->my_stats->original_file_size,best_return_struct->my_stats->compressed_file_size);
			close(stats_fd);

			char *encoded_file_name = best_return_struct->encoded_file_name;
			char *extra_file_name = best_return_struct->extra_file_name;

			size_t stats_file_length = get_file_length("server_stats.txt");
			size_t encoded_file_length = get_file_length(encoded_file_name);
			size_t extra_file_length = get_file_length(extra_file_name);

			send_response_length(clientfd, stats_file_length);
			send_response_length(clientfd, encoded_file_length);
			send_response_length(clientfd, extra_file_length);

			fprintf(stderr, "[SERVER] sent stats file size (%zu)\n", stats_file_length);
			fprintf(stderr, "[SERVER] sent encoded file size (%zu)\n", encoded_file_length);
			fprintf(stderr, "[SERVER] sent extra file size (%zu)\n", extra_file_length);

			stats_fd = open("server_stats.txt", O_CREAT | O_RDWR, 0644);
			read_fd_write_fd(stats_fd, clientfd, stats_file_length);

			// fprintf(stderr, "%s\n", encoded_file_name);
			int encoded_file_fd = open(encoded_file_name, O_RDWR);
			read_fd_write_fd(encoded_file_fd, clientfd, encoded_file_length);

			if (extra_file_name) {
				int extra_file_fd = open(extra_file_name, O_RDWR);
				read_fd_write_fd(extra_file_fd, clientfd, extra_file_length);
			}

			remove("huffman_tree.txt");
			remove("huffman_encoded.txt");
			remove("l7w_encoded.txt");
			remove("run_length_encoded.txt");
			remove(TEMP_FILE);
			close(clientfd);

		}

		if(shutdown(sockfd, SHUT_RDWR)){
			perror(NULL);
			exit(1);
		}
		freeaddrinfo(result);
}

int main(int argc, char **argv) {
		if (argc != 2) {
				fprintf(stderr, "./server <port>\n");
				return -1;
		}

		struct sigaction act;
		memset(&act, '\0', sizeof(act));
		act.sa_handler = close_server;
		if (sigaction(SIGINT, &act, NULL) < 0) {
				perror("sigaction");
				return 1;
		}

		signal(SIGINT, close_server);
		run_server(argv[1]);
		cleanup();
		pthread_exit(NULL);
}