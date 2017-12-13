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
#include <unistd.h>


#define MAX_CLIENTS 8

#define TEMP_FILE "TEMP_FILE"
#define NUM_ALGORITHMS 3
#define THREAD_COUNT 8

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

void read_fd_write_fd(int readfd, int writefd, size_t len) {
    char buf[1025];
    size_t read_so_far = 0;
    while(read_so_far < len) {
        int ret = read(readfd,buf + read_so_far, 1024);
        if (ret > 0) {
            buf[ret] = '\0';
            dprintf(writefd,"%s",buf);
            read_so_far += ret;
        }
    }
}

size_t get_response_length(int sockfd) {
		size_t len;
		read_complete(sockfd, &len, sizeof(len));
		return len;
}

void send_response_length(int sockfd, size_t len) {
	write(sockfd, &len, sizeof(len));
}


void cleanup() {
		if (shutdown(serverSocket, SHUT_RDWR) != 0) {
				perror("shutdown():");
		}
		close(serverSocket);

		for (int i = 0; i < MAX_CLIENTS; i++) {
				if (clients[i] != -1) {
						if (shutdown(clients[i], SHUT_RDWR) != 0) {
								perror("shutdown(): ");
						}
						close(clients[i]);
				}
		}
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
				execl(HUFFMAN_ENCODE_EXE, HUFFMAN_ENCODE_EXE,my_worker_struct->file_name,encoded_file_name,tree_file_name,NULL);
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
				execl(RUN_LENGTH_EXE, RUN_LENGTH_EXE, 1, my_worker_struct->file_name, encoded_file_name, NULL);
				fprintf(stderr, "%s\n", EXEC_ERR);
				exit(1);
		}

		stat_struct *my_stat_struct = malloc(sizeof(stat_struct));
		my_stat_struct->original_file_size = get_file_length(my_worker_struct->file_name);
		my_stat_struct->compressed_file_size = get_file_length(encoded_file_name);

		return_struct *my_return_struct = malloc(sizeof(return_struct));
		my_return_struct->my_stats = my_stat_struct;
		my_return_struct->encoded_file_name = NULL;
		my_return_struct->extra_file_name = NULL;

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
		execl(L7W_EXE, L7W_EXE, 1, my_worker_struct->file_name, encoded_file_name, NULL;)
		fprintf(stderr, "%s\n", EXEC_ERR);
		exit(1);
	}
		stat_struct *my_stat_struct = malloc(sizeof(stat_struct));
		my_stat_struct->original_file_size = get_file_length(my_worker_struct->file_name);
		my_stat_struct->compressed_file_size = get_file_length(encoded_file_name);

		return_struct *my_return_struct = malloc(sizeof(return_struct));
		my_return_struct->my_stats = my_stat_struct;
		my_return_struct->encoded_file_name = NULL;
		my_return_struct->extra_file_name = NULL;

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
		fillWithInt();
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
		initIndexesArr(indexes);
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

			size_t size_of_file = get_response_length(clientfd);
			int original_fd = open("original.txt", O_CREAT, O_RDWR);
			read_fd_write_fd(clientfd, original, size_of_file);

			pthread_t threads[3];
			worker_struct *my_worker_struct = malloc(sizeof(worker_struct));

			my_worker_struct->file_name = TEMP_FILE;
			pthread_create(threads, NULL, huffman_worker, my_worker_struct);
			pthread_create(threads + 1, NULL, run_length_worker, my_worker_struct);
			pthread_create(threads + 2, NULL, l7w_worker, my_worker_struct);

			return_struct return_struct_arr[NUM_ALGORITHMS];
			for (int i = 0; i < NUM_ALGORITHMS; ++i) {
				 pthread_join(threads[i], &return_struct_arr[i]);
			}

			return_struct best_return_struct = return_struct_arr[0];
			for (int i = 1; i < NUM_ALGORITHMS; ++i) {
				if (return_struct_arr[i].my_stats->compressed_file_size < best_return_struct.my_stats->compressed_file_size) {
					best_return_struct = return_struct_arr[i];
				}
			}

			//create file with statistics
			int have_extra_file = best_return_struct.extra_file_name != NULL;
			//send back best file
			send_response_length(clientfd, best_return_struct->my_stats.compressed_file_size);
			if (have_extra_file) {
				send_response_length(clientfd, get_file_length(best_return_struct.extra_file_name));
			} else {
				send_response_length(clientfd, 0);
			}
			send_response_length(clientfd, 0); //send back stat file

			int encoded_fd = open(best_return_struct.encoded_file_name, O_WR);
			read_fd_write_fd(encoded_fd, clientfd);

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