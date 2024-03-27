/**
 * @file main.c
 * @brief Main file for the server application
 * 
 * This file contains the main function for the server application, the function ran by the server threads and the signal handler
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path src/srv/main.c
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
#include <signal.h>
#include <errno.h>
#include <stdbool.h>

#include "../inc/srv/main.h"
#include "common.h"
#include "args.h"

#include "server_socket.h"
#include "server_threads.h"
#include "server_queues.h"

#include "queue_thread_safe.h"

volatile sig_atomic_t term_flag = 1;
//pthread_mutex_t term_flag_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    // Disable buffering for stdout and stderr
    disable_buffering();

	struct gengetopt_args_info args;
	if (cmdline_parser(argc, argv, &args)) {
        perror("Command line parser error\n");
        exit(EXIT_FAILURE);
	}

    char *time = NULL;
    if (get_current_time(&time) == STATUS_ERROR) {
        fprintf(stderr, "Could not get current time\n");
        exit(EXIT_FAILURE);
    }
    printf("Server started at %s\n", time);
    free(time);
    

    int server_port = args.port_arg;   
    if (validate_port(server_port) == STATUS_ERROR) {
        fprintf(stderr, "Wrong port value inserted\n");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa; 
    memset(&sa, 0, sizeof(struct sigaction));
    if (init_signal_handlers(&sa) == STATUS_ERROR) {
        fprintf(stderr, "Could not initialize signal handlers\n");
        exit(EXIT_FAILURE);
    }

    int server_socket = -1;
    if (init_server_socket(server_port, &server_socket) == STATUS_ERROR) {
        fprintf(stderr, "Could not initialize the socket\n");
        exit(EXIT_FAILURE);
    }

    logs_file_t server_logs_file;
    memset(&server_logs_file, 0, sizeof(logs_file_t));
    if (open_logs_file(&server_logs_file, SERVER_LOGS_FILE) == STATUS_ERROR) {
        fprintf(stderr, "Could not open the server logs file\n");
        close_socket(server_socket);
        exit(EXIT_FAILURE);
    }

    queue_thread_safe_t *queues[NUMBER_OF_SENSORS];
    memset(queues, 0, sizeof(queue_thread_safe_t *) * NUMBER_OF_SENSORS);
    if (create_queues(queues) == STATUS_ERROR) {
        fprintf(stderr, "Could not create all necessary queues\n");
        close_socket(server_socket);
        close_logs_file(&server_logs_file);
        exit(EXIT_FAILURE);
    }

    pthread_t tids[NUMBER_OF_SENSORS];
    memset(tids, 0, sizeof(pthread_t) * NUMBER_OF_SENSORS);
    server_thread_params_t thread_params[NUMBER_OF_SENSORS];
    memset(thread_params, 0, sizeof(server_thread_params_t) * NUMBER_OF_SENSORS);
    if (init_server_threads(tids, thread_params, server_socket, &server_logs_file, queues, handle_client) == STATUS_ERROR) {
        fprintf(stderr, "Could not initialize all threads\n");
        close_socket(server_socket);
        close_logs_file(&server_logs_file);
        destroy_queues(queues);
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening for UDP messages on port %d\n", server_port);    

    uint8_t buffer[MAX_BUFFER_SIZE];

    //queue_thread_safe_t *queue_zero = queues[0];

    while (term_flag) { //Using the printf/fprintf to write to stdout/stderr is too slow 

        memset(buffer,0,sizeof(buffer));

        if (receive_from_socket(server_socket, buffer) == STATUS_ERROR) {
            if (errno == EINTR) { // Interrupted by a signal 
                continue;
            }
            fprintf(stderr, "Could not read data from the socket\n");
            continue;
        }

        proto_sensor_data_t *data = calloc(1, sizeof(proto_sensor_data_t)); // The corresponding thread will free this memory
        if (data == NULL) {
            fprintf(stderr, "Could not allocate memory to store sensor data\n");
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

    printf("\nWaiting for threads to empty queues...\n");
    if (join_threads(tids) == STATUS_ERROR) {
        fprintf(stderr, "Could not join all threads\n");
        close_socket(server_socket);
        close_logs_file(&server_logs_file);
        destroy_queues(queues);
        exit(EXIT_FAILURE);
    }
    printf("Threads finished emptying queues and ended\n\n");


    uint32_t total_messages = 0;
    float total_sum = 0;
    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++){
        uint32_t counter = thread_params[i].counter;
        uint32_t tid = thread_params[i].id;
        printf("Thread %d received %d messages from sensor %d\n", tid, counter, i);

        float result = 0;

        if (counter > 0) {
            result = (float) (thread_params[i].sum / (float) counter);
        }

        printf("Thread %d received an average value of %.4f from sensor %d\n", tid, result, i);
        total_messages += counter;
        total_sum += thread_params[i].sum;
    }

    printf("Total messages received by all threads: %d\n", total_messages);
    float total_result = 0;
    if (total_messages > 0) {
        total_result = (float) (total_sum / (float) total_messages);
    }
    printf("Average value received by all threads: %.4f\n", total_result);
    

    if (close_socket(server_socket) == STATUS_ERROR) {
        fprintf(stderr, "Could not close the socket\n");
        exit(EXIT_FAILURE);
    }

    if (close_logs_file(&server_logs_file) == STATUS_ERROR) {
        fprintf(stderr, "Could not close the server logs file\n");
        exit(EXIT_FAILURE);
    }

    destroy_queues(queues);

	cmdline_parser_free(&args);

    printf("\nTerminating server\n");
    
    return 0;
}

void *handle_client(void *arg){ //TODO
    //TODO

    server_thread_params_t *params = (server_thread_params_t *) arg;
    //printf("Thread %d is waiting\n", params->id);

    
    uint32_t id = params->id;
    //int server_socket = params->server_socket;
    queue_thread_safe_t *queue = params->queue;
    logs_file_t *server_logs_file = params->server_logs_file;

    proto_sensor_data_t *data = NULL;

    while (term_flag || queue_get_number_of_elements_thread_safe(queue) > 0) {

        if (queue_get_number_of_elements_thread_safe(queue) == 0) {
            //printf("No data in the queue %d\n", id);
            continue;
        }

        data = queue_remove_thread_safe(queue);
        if (data == NULL ) {
            //printf("No data in the queue %d\n", id);
            continue;
        }

        if (log_server_sensor_data(server_logs_file, data, id) == STATUS_ERROR) {
            fprintf(stderr, "Could not log sensor data\n");
            free(data);
            continue;
        }
        
        params->counter++;
        params->sum += get_float_value(data);

        //sleep(1);

        free(data);  
        data = NULL;      
    }

    return NULL;
}

int init_signal_handlers(struct sigaction *sa) {
    if (sa == NULL) {
        fprintf(stderr, "Invalid signal action structure pointer\n");
        return STATUS_ERROR;
    }

    sa->sa_handler = signal_handler;
    sigemptyset(&sa->sa_mask);
    sa->sa_flags = 0;
    // sa->sa_flags |= SA_RESTART; // Optionally, uncomment this line if you want to enable automatic restart of interrupted system calls

    if (sigaction(SIGTERM, sa, NULL) == -1) {
        fprintf(stderr, "Initialization of SIGTERM failed\n");
        return STATUS_ERROR;
    }

    if (sigaction(SIGINT, sa, NULL) == -1) {
        fprintf(stderr, "Initialization of SIGINT failed\n");
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

void signal_handler(int signum) {
    int aux;	
	aux = errno;   
	
	printf("\nSignal Received (%d)\n", signum);	

    //pthread_mutex_lock(&term_flag_mutex);
    term_flag = 0;	
    //pthread_mutex_unlock(&term_flag_mutex);
	
	errno = aux;   
}

// Path: src/srv/main.c