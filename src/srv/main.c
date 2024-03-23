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
    if (init_threads(&tids, &thread_params, server_socket) == STATUS_ERROR) {
        fprintf(stderr, "Error initializing threads\n");
        exit(EXIT_FAILURE);
    }

    sleep(10);

    /*
    socklen_t udp_client_endpoint_length = sizeof(struct sockaddr_in);
	struct sockaddr_in client_endpoint = {0};
	ssize_t read_bytes;
    uint8_t buffer[MAX_BUFFER_SIZE] = {0};

    
    while (1) {
        memset(buffer,0,sizeof(buffer));

        printf("Waiting for a client message...\n");
        if ((read_bytes = recvfrom(server_socket, buffer, sizeof(uint8_t) * MAX_BUFFER_SIZE , 0, (struct sockaddr *) &client_endpoint, &udp_client_endpoint_length)) == -1) {
            fprintf(stderr, "Error receiving data from client\n");
            exit(EXIT_FAILURE);
        }
        printf("Received %ld bytes from %s:%d - %s\n", read_bytes, inet_ntoa(client_endpoint.sin_addr), ntohs(client_endpoint.sin_port), buffer);
    }
    */

    free(tids);
    free(thread_params);

    if (close_server_socket(server_socket) == STATUS_ERROR) {
        fprintf(stderr, "Could not close the socket\n");
        exit(EXIT_FAILURE);
    }


	cmdline_parser_free(&args);
    return 0;
}

void main_server(void) {
    printf("Main server\n");
}
