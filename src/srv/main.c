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
#include "server_queues.h"
#include "server_logs.h"

#include "queue_thread_safe.h"

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    // Disable buffering for stdout and stderr
    disable_buffering();
    
	struct gengetopt_args_info args;
	if (cmdline_parser(argc, argv, &args)) {
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
        fprintf(stderr, "Could not initialize the socket\n");
        exit(EXIT_FAILURE);
    }

    logs_file_t server_logs_file;
    if (open_server_logs_file(&server_logs_file, SERVER_LOGS_FILE) == STATUS_ERROR) {
        fprintf(stderr, "Could not open the server logs file\n");
        close_socket(server_socket);
        exit(EXIT_FAILURE);
    }

    queue_thread_safe_t *queues[NUMBER_OF_SENSORS];
    memset(queues, 0, sizeof(queue_thread_safe_t *) * NUMBER_OF_SENSORS);
    if (create_queues(queues) == STATUS_ERROR) {
        fprintf(stderr, "Could not create all necessary queues\n");
        close_socket(server_socket);
        close_server_logs_file(&server_logs_file);
        exit(EXIT_FAILURE);
    }

    pthread_t tids[NUMBER_OF_SENSORS];
    memset(tids, 0, sizeof(pthread_t) * NUMBER_OF_SENSORS);
    server_thread_params_t thread_params[NUMBER_OF_SENSORS];
    memset(thread_params, 0, sizeof(server_thread_params_t) * NUMBER_OF_SENSORS);
    if (init_server_threads(tids, thread_params, server_socket, &server_logs_file, queues, handle_client) == STATUS_ERROR) {
        fprintf(stderr, "Could not initialize all threads\n");
        close_socket(server_socket);
        close_server_logs_file(&server_logs_file);
        destroy_queues(queues);
        exit(EXIT_FAILURE);
    }
    
    uint8_t buffer[MAX_BUFFER_SIZE];

    //queue_thread_safe_t *queue_zero = queues[0];

    while(1){ //Using the printf/fprintf to write to stdout/stderr is too slow 

        memset(buffer,0,sizeof(buffer));

        if (receive_from_socket(server_socket, buffer) == STATUS_ERROR) {
            fprintf(stderr, "Could not read data from the socket\n");
            continue;
        }

        proto_sensor_data_t *data = calloc(1, sizeof(proto_sensor_data_t)); // The corresponding thread will free this memory
        if (data == NULL) {
            fprintf(stderr, "Could not allocate memory to store data\n");
            continue;
        }

        deserialize_sensor_data(buffer, data);

        if (data->hdr.type != PROTO_SENSOR_DATA) {
            fprintf(stderr, "Invalid message type\n");
            continue;
        }

        if (data->hdr.sensor_id >= NUMBER_OF_SENSORS) {
            fprintf(stderr, "Invalid sensor ID\n");
            continue;
        }

        //printf("Sensor with the ID %d sent the value %f\n", data->hdr.sensor_id, get_float_value(data) );

        queue_insert_thread_safe(data, queues[data->hdr.sensor_id]);

        //print_queue(queues[data->hdr.sensor_id]);

        /*
        for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++){
            printf("Queue %d: %d\n", i, queue_get_number_of_elements_thread_safe(queues[i]));
        }
        */

    }

    if (close_socket(server_socket) == STATUS_ERROR) {
        fprintf(stderr, "Could not close the socket\n");
        exit(EXIT_FAILURE);
    }

    if (close_server_logs_file(&server_logs_file) == STATUS_ERROR) {
        fprintf(stderr, "Could not close the server logs file\n");
        exit(EXIT_FAILURE);
    }

    destroy_queues(queues);

	cmdline_parser_free(&args);
    return 0;
}

void *handle_client(void *arg){ //TODO
    //TODO

    server_thread_params_t *params = (server_thread_params_t *) arg;
    //printf("Thread %d is waiting\n", params->id);

    //int server_socket = params->server_socket;
    uint32_t id = params->id;
    queue_thread_safe_t *queue = params->queue;
    logs_file_t *server_logs_file = params->server_logs_file;

    proto_sensor_data_t *data = NULL;

    //int count = 0;

    while(1){
        data = queue_remove_thread_safe(queue);
        if (data == NULL) {
            //printf("No data in the queue %d\n", id);
            continue;
        }

        //count++;
        //printf("Element removed from the queue %d (Count: %d)\n", id, count);
        //printf("q %d C: %d\n", id, count);

        if (log_server_sensor_data(server_logs_file, data, id) == STATUS_ERROR) {
            fprintf(stderr, "Could not log sensor data\n");
            free(data);
            continue;
        }

        //sleep(2);

        free(data);
    }
    return NULL;
}
