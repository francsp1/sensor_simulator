#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "../inc/srv/main.h"
#include "common.h"
#include "args.h"

#include "socket.h"
#include "threads.h"


int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    // Disable buffering for stdout
    setvbuf(stdout, NULL, _IONBF, 0);

    // Disable buffering for stderr
    setvbuf(stderr, NULL, _IONBF, 0);
    
	struct gengetopt_args_info args;
	if (cmdline_parser(argc, argv, &args)){
        perror("Command line parser error\n");
        exit(EXIT_FAILURE);
	}

    int server_port = args.port_arg;   
    if (validate_port(server_port) == STATUS_ERROR) {
        fprintf(stderr, "Wrong port value inserted\n");
        exit(EXIT_FAILURE);
    }

    int server_socket = -1;
    if (init_server_socket(server_port, &server_socket) == STATUS_ERROR) {
        fprintf(stderr, "Error initializing socket\n");
        exit(EXIT_FAILURE);
    }

    pthread_t *tids = NULL;
    thread_params_t *thread_params = NULL;
    if (init_threads(&tids, &thread_params, server_socket, handle_client) == STATUS_ERROR) {
        fprintf(stderr, "Error initializing threads\n");
        close_server_socket(server_socket);
        exit(EXIT_FAILURE);
    }

    /*
    if (join_threads(tids) == STATUS_ERROR) {
        fprintf(stderr, "Could not join all threads\n");
        close_server_socket(server_socket);
        free_threads(&tids, &thread_params);
        exit(EXIT_FAILURE);
    }
    */
    while(1){
        sleep(1);
    }

    free_threads(&tids, &thread_params);

    if (close_server_socket(server_socket) == STATUS_ERROR) {
        fprintf(stderr, "Could not close the socket\n");
        exit(EXIT_FAILURE);
    }


	cmdline_parser_free(&args);
    return 0;
}

void *handle_client(void *arg){

    thread_params_t *params = (thread_params_t *) arg;

    socklen_t client_endpoint_length = sizeof(struct sockaddr_in);
	struct sockaddr_in client_endpoint;
    memset(&client_endpoint, 0, sizeof(struct sockaddr_in));
	ssize_t read_bytes;
    uint8_t buffer[MAX_BUFFER_SIZE];

    while(1){
        memset(buffer,0,sizeof(buffer));

        printf("Thread %d is waiting for a client message...\n", params->id);

        if ((read_bytes = recvfrom(params->server_socket, buffer, (sizeof(uint8_t) * MAX_BUFFER_SIZE) - 1 , 0, (struct sockaddr *) &client_endpoint, &client_endpoint_length)) == -1) {
            fprintf(stderr, "Error receiving data from client\n");
            continue;
        }
        printf("Thread %d received %ld bytes from %s:%d - %s\n", params->id, read_bytes, inet_ntoa(client_endpoint.sin_addr), ntohs(client_endpoint.sin_port), buffer);

    }

    return NULL;
}
