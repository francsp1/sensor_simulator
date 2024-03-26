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
    struct sockaddr_in server_endpoint;
    memset(&server_endpoint, 0, sizeof(struct sockaddr_in));
    if (init_client_socket(args.ip_arg, server_port, &client_socket, &server_endpoint) == STATUS_ERROR) {
        fprintf(stderr, "Error initializing socket\n");
        exit(EXIT_FAILURE);
    }

    logs_file_t client_logs_file;
    memset(&client_logs_file, 0, sizeof(logs_file_t));
    if (open_logs_file(&client_logs_file, CLIENT_LOGS_FILE) == STATUS_ERROR) {
        fprintf(stderr, "Could not open the server logs file\n");
        close_socket(client_socket);
        exit(EXIT_FAILURE);
    }

    pthread_t tids[NUMBER_OF_SENSORS];
    memset(tids, 0, sizeof(pthread_t) * NUMBER_OF_SENSORS);
    client_thread_params_t thread_params[NUMBER_OF_SENSORS];
    memset(thread_params, 0, sizeof(client_thread_params_t) * NUMBER_OF_SENSORS);
    if (init_client_threads(tids, thread_params, client_socket, &server_endpoint, handle_server) == STATUS_ERROR) {
        fprintf(stderr, "Could not initialize all threads\n");
        close_socket(client_socket);
        close_logs_file(&client_logs_file);
        exit(EXIT_FAILURE);
    }

    if (join_threads(tids) == STATUS_ERROR) {
        fprintf(stderr, "Could not join all threads\n");
        close_socket(client_socket);
        close_logs_file(&client_logs_file);
        exit(EXIT_FAILURE);
    }

    if (close_socket(client_socket) == STATUS_ERROR) {
        fprintf(stderr, "Error closing socket\n");
        exit(EXIT_FAILURE);
    }

    if (close_logs_file(&client_logs_file) == STATUS_ERROR) {
        fprintf(stderr, "Error closing logs file\n");
        exit(EXIT_FAILURE);
    }

    cmdline_parser_free(&args);

    return 0;
}

void *handle_server(void *arg){ //TODO
    //TODO
    client_thread_params_t *params = (client_thread_params_t *) arg;
    
    struct sockaddr_in *server_endpoint = params->server_endpoint;
    uint32_t id = params->id;
    int client_socket = params->client_socket;

    struct timespec delay = {0};
    delay.tv_sec = 0;
    delay.tv_nsec = DELAY_MS * 1000000; // Convert milliseconds to nanoseconds

    proto_sensor_data_t data;

    int aux = 50;
    
    while (aux) {
        memset(&data, 0, sizeof(proto_sensor_data_t));
        serialize_sensor_data(&data, id);

        if (send_to_socket(client_socket, &data, server_endpoint) == STATUS_ERROR) {
            fprintf(stderr, "Could not send data to server\n");
            continue;
        }

        nanosleep(&delay, NULL);

        aux--;
    }

    printf("Thread %d finished\n", id);
  
    return NULL;
}    

/*
    printf("Debug data\n");
    printf("data.hdr.type: %d\n", ntohl(data.hdr.type));
    printf("data.hdr.sensor_id: %d\n", ntohl(data.hdr.sensor_id));
    printf("data.hdr.len: %d\n", ntohs(data.hdr.len));
    */

/*

    for (uint32_t i = 0; i < 100; i++){
        memset(&data, 0, sizeof(proto_sensor_data_t));
        serialize_sensor_data(&data, id);

        printf("i: %d\n", i);
        if (send_to_socket(client_socket, &data, server_endpoint) == STATUS_ERROR) {
            fprintf(stderr, "Could not send data to server\n");
            continue;
        }
    }
*/