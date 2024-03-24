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

#include "../inc/cli/main.h"
#include "common.h"
#include "args.h"

#include "client_socket.h"
#include "client_threads.h"


int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    disable_buffering();
    
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
    
    int client_socket = -1;
    socklen_t server_endpoint_length = sizeof(struct sockaddr_in);	
    struct sockaddr_in server_endpoint;
    memset(&server_endpoint, 0, sizeof(struct sockaddr_in));
    if (init_client_socket(args.ip_arg, server_port, &client_socket, &server_endpoint) == STATUS_ERROR) {
        fprintf(stderr, "Error initializing socket\n");
        exit(EXIT_FAILURE);
    }

    pthread_t tids[NUMBER_OF_SENSORS];
    memset(tids, 0, sizeof(pthread_t) * NUMBER_OF_SENSORS);
    client_thread_params_t thread_params[NUMBER_OF_SENSORS];
    memset(thread_params, 0, sizeof(client_thread_params_t) * NUMBER_OF_SENSORS);
    if (init_client_threads(tids, thread_params, client_socket, handle_server) == STATUS_ERROR) {
        fprintf(stderr, "Could not initialize all threads\n");
        close_socket(client_socket);
        exit(EXIT_FAILURE);
    }

    proto_send_data_t data;
    memset(&data, 0, sizeof(proto_send_data_t));
    serialize_data(&data);

    /*
    printf("Debug data\n");
    printf("data.hdr.type: %d\n", ntohl(data.hdr.type));
    printf("data.hdr.sensor_id: %d\n", ntohl(data.hdr.sensor_id));
    printf("data.hdr.len: %d\n", ntohs(data.hdr.len));
    */

    if (send_to_socket(client_socket, &data, server_endpoint, server_endpoint_length) == STATUS_ERROR) {
        fprintf(stderr, "Could not send data to server\n");
        exit(EXIT_FAILURE);
    }

    if (close_socket(client_socket) == STATUS_ERROR) {
        fprintf(stderr, "Error closing socket\n");
        exit(EXIT_FAILURE);
    }
   
    cmdline_parser_free(&args);

    return 0;
}

void *handle_server(void *arg){ //TODO
    //TODO

    client_thread_params_t *params = (client_thread_params_t *) arg;
    //printf("Thread %d is waiting\n", params->id);
    (void)params;

    while(1){
        sleep(1);
    }
    return NULL;
}