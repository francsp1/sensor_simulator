/**
 * @file main.c
 * @brief Main file for the client application
 * 
 * This file contains the main function for the client application and the function ran by the client threads
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path src/cli/main.c
 */
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

    int packets_per_sensor = args.packets_per_sensor_arg;
    if (packets_per_sensor <= 0) {
        fprintf(stderr, "Error: --packets_per_sensor (-s) must receive a positive value\n");
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
    if (init_client_threads(tids, thread_params, client_socket, &server_endpoint, &client_logs_file, packets_per_sensor, handle_server) == STATUS_ERROR) {
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

void *handle_server(void *arg) { 
    client_thread_params_t *params = (client_thread_params_t *) arg;
    
    struct sockaddr_in *server_endpoint = params->server_endpoint;
    uint32_t id = params->id;
    int client_socket = params->client_socket;
    logs_file_t *client_logs_file = params->client_logs_file;
    int packets_per_thread = params->packets_per_sensor;

    struct timespec delay = {0};
    delay.tv_sec = 0;
    delay.tv_nsec = DELAY_MS * 1000000; // Convert milliseconds to nanoseconds

    proto_sensor_data_t data;
    proto_sensor_data_t serialized_data;

    srand(time(NULL) * id);
    
    while (packets_per_thread) {
        memset(&data, 0, sizeof(proto_sensor_data_t));
        memset(&serialized_data, 0, sizeof(proto_sensor_data_t));

        if (pack_sensor_data(&data, id) == STATUS_ERROR) {
            fprintf(stderr, "Could not pack sensor data\n");
            continue;
        }

        if (serialize_sensor_data(&data, &serialized_data) == STATUS_ERROR) {
            fprintf(stderr, "Could not serialize sensor data\n");
            continue;
        }

        if (send_to_socket(client_socket, &serialized_data, server_endpoint) == STATUS_ERROR) {
            fprintf(stderr, "Could not send data to server\n");
            continue;
        }

        if (log_client_sensor_data(client_logs_file, &data, id) == STATUS_ERROR) {
            fprintf(stderr, "Could not log sensor data\n");
            continue;
        }

        nanosleep(&delay, NULL);

        packets_per_thread--;
    }

    printf("Thread %d finished\n", id);
  
    return NULL;
}    

// Path: src/cli/main.c