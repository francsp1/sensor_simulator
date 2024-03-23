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

#include "server_socket.h"
#include "server_threads.h"


int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    // Disable buffering for stdout and stderr
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

    int server_socket = -1;
    if (init_server_socket(server_port, &server_socket) == STATUS_ERROR) {
        fprintf(stderr, "Error initializing socket\n");
        exit(EXIT_FAILURE);
    }

    pthread_t tids[NUMBER_OF_SENSORS];
    memset(tids, 0, sizeof(pthread_t) * NUMBER_OF_SENSORS);
    thread_params_t thread_params[NUMBER_OF_SENSORS];
    memset(thread_params, 0, sizeof(thread_params_t) * NUMBER_OF_SENSORS);
    if (init_threads(tids, thread_params, server_socket, handle_client) == STATUS_ERROR) {
        fprintf(stderr, "Could not initialize all threads\n");
        close_socket(server_socket);
        exit(EXIT_FAILURE);
    }
    
    uint8_t buffer[MAX_BUFFER_SIZE];

    while(1){
        memset(buffer,0,sizeof(buffer));

        if (receive_from_socket(server_socket, buffer) == STATUS_ERROR) {
            fprintf(stderr, "Could not read data from the socket\n");
            continue;
        }

        proto_send_data_t data;
        memset(&data, 0, sizeof(proto_send_data_t));
        deserialize_data(buffer, &data);

        if (data.hdr.type != PROTO_SEND_DATA) {
            fprintf(stderr, "Invalid message type\n");
            continue;
        }

        printf("Sensor with the ID %d sent the value %f\n", data.hdr.sensor_id, get_float_value(data) );

    }

    if (close_socket(server_socket) == STATUS_ERROR) {
        fprintf(stderr, "Could not close the socket\n");
        exit(EXIT_FAILURE);
    }


	cmdline_parser_free(&args);
    return 0;
}

void *handle_client(void *arg){ //TODO
    //TODO

    thread_params_t *params = (thread_params_t *) arg;
    //printf("Thread %d is waiting\n", params->id);
    (void)params;

    while(1){
        sleep(1);
    }
    return NULL;
}
