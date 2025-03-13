/**
 * @file main.c
 * @brief Main file for the server application
 * 
 * This file contains the main function for the server application, the function ran by the server threads and the signal handler function
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
#include <execinfo.h>
#include <stdatomic.h>

#include "../inc/srv/main.h"
#include "common.h"
#include "args.h"

#include "server_socket.h"
#include "server_threads.h"
#include "server_queues.h"

#include "queue_thread_safe.h"

volatile sig_atomic_t keep_running = 1;

int main(int argc, char *argv[]) {
    //For testing, print sizeof proto_type_e
    //printf("Size of proto_type_e: %ld\n", sizeof(proto_type_e));

    atomic_bool main_thread_done = ATOMIC_VAR_INIT(false);

    struct sigaction sa; memset(&sa, 0, sizeof(struct sigaction));
    if (init_signal_handlers(&sa) == STATUS_ERROR) {
        fprintf(stderr, "Could not initialize signal handlers\n");
        exit(EXIT_FAILURE);
    }

    // Disable buffering for stdout and stderr
    disable_buffering();

	struct gengetopt_args_info args; memset(&args, 0, sizeof(struct gengetopt_args_info));
	if (cmdline_parser(argc, argv, &args)) {
        perror("Command line parser error\n");
        exit(EXIT_FAILURE);
	}

    char *time = NULL;
    if (get_current_time(&time) == STATUS_ERROR) {
        fprintf(stderr, "Could not get current time\n");
        cmdline_parser_free(&args);
        exit(EXIT_FAILURE);
    }
    printf("Server started at %s\n", time);
    free(time);
    

    int server_port = args.port_arg;   
    if (validate_port(server_port) == STATUS_ERROR) {
        fprintf(stderr, "Wrong port value inserted\n");
        cmdline_parser_free(&args);
        exit(EXIT_FAILURE);
    }

    int server_socket = -1;
    if (init_server_socket(server_port, &server_socket) == STATUS_ERROR) {
        fprintf(stderr, "Could not initialize the socket\n");
        cmdline_parser_free(&args);
        exit(EXIT_FAILURE);
    }

    int logs_files_flag = args.logs_files_flag;
    logs_file_t server_logs_files[NUMBER_OF_SENSORS]; memset(server_logs_files, 0, sizeof(logs_file_t) * NUMBER_OF_SENSORS);
    if (open_server_logs_files(logs_files_flag, server_logs_files) == STATUS_ERROR) {
        fprintf(stderr, "Could not open all the server logs files\n");
        cmdline_parser_free(&args);
        close_socket(server_socket);
        exit(EXIT_FAILURE);
    }

    queue_thread_safe_t *queues[NUMBER_OF_SENSORS]; memset(queues, 0, sizeof(queue_thread_safe_t *) * NUMBER_OF_SENSORS);
    if (create_queues(queues) == STATUS_ERROR) {
        fprintf(stderr, "Could not create all necessary queues\n");
        cmdline_parser_free(&args);
        close_socket(server_socket);
        close_logs_files(logs_files_flag, server_logs_files);
        exit(EXIT_FAILURE);
    }

    pthread_t tids[NUMBER_OF_SENSORS]; memset(tids, 0, sizeof(pthread_t) * NUMBER_OF_SENSORS);
    server_thread_params_t thread_params[NUMBER_OF_SENSORS]; memset(thread_params, 0, sizeof(server_thread_params_t) * NUMBER_OF_SENSORS);
    if (init_server_threads(tids, thread_params, server_socket, logs_files_flag, server_logs_files, queues, &main_thread_done, handle_client) == STATUS_ERROR) {
        fprintf(stderr, "Could not initialize all threads\n");
        cmdline_parser_free(&args);
        close_socket(server_socket);
        close_logs_files(logs_files_flag, server_logs_files);
        destroy_queues(queues);
        exit(EXIT_FAILURE);
    }

    printf("Server listening for UDP messages on port %d\n", server_port);   

    uint8_t buffer[MAX_BUFFER_SIZE]; memset(buffer, 0, sizeof(buffer));
    proto_sensor_data_t *data = NULL;
    while (keep_running) { //Using the printf/fprintf to write to stdout/stderr is too slow 
        
        if (receive_from_socket(server_socket, buffer) == STATUS_ERROR) {
            /*
            if (errno == EINTR) { // Interrupted by a signal
                fprintf(stderr, "recvfrom() interrupted by a signal\n");
                continue;
            }
            */
            fprintf(stderr, "Could not read data from the socket\n");
            continue;
        }
        
        data = calloc(1, sizeof(proto_sensor_data_t )); // The corresponding thread will free this memory
        if (data == NULL) {
            fprintf(stderr, "Could not allocate memory to store sensor data\n");
            continue;
        }

        deserialize_sensor_data(buffer, data);

        if (data->hdr.type != PROTO_SENSOR_DATA) {
            fprintf(stderr, "Invalid message type\n");
            free(data);
            continue;
        }

        if (data->hdr.sensor_id >= NUMBER_OF_SENSORS) {
            fprintf(stderr, "Invalid sensor ID\n");
            free(data);
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
        memset(buffer,0,sizeof(buffer));
    }

    atomic_store_explicit(&main_thread_done, true, memory_order_release);

    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++) {
        pthread_cond_broadcast(&(queues[i]->cond));
    }

    printf("\nWaiting for threads to empty queues...\n");
    if (join_threads(tids) == STATUS_ERROR) {
        fprintf(stderr, "Could not join all threads\n");
        close_socket(server_socket);
        close_logs_files(logs_files_flag, server_logs_files);
        destroy_queues(queues);
        exit(EXIT_FAILURE);
    }
    printf("Threads finished emptying queues and ended\n\n");


    uint32_t total_messages = 0;
    float total_sum = 0;
    uint32_t counter = 0;
    uint32_t tid = 0;
    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++) {
        counter = thread_params[i].counter;
        tid = thread_params[i].id;
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

    if (close_logs_files(logs_files_flag, server_logs_files) == STATUS_ERROR) {
        fprintf(stderr, "Could not close the server logs file\n");
        exit(EXIT_FAILURE);
    }

    /*
    //print all queues before destroying them
    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++){
        printf("Queue %d\n", i);
        print_queue(queues[i]);
    }
    */

    destroy_queues(queues);

	cmdline_parser_free(&args);

    printf("\nTerminating server\n");
    
    return 0;
}

void *handle_client(void *arg) { 
    // Block SIGINT in the worker threads
    sigset_t mask; memset(&mask, 0, sizeof(sigset_t));
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    server_thread_params_t *params = (server_thread_params_t *) arg;
    //printf("Thread %d is waiting\n", params->id);
    
    uint32_t id = params->id;
    //int server_socket = params->server_socket;
    queue_thread_safe_t *queue = params->queue;
    logs_file_t *server_logs_file = params->server_logs_file;
    atomic_bool *main_thread_done = params->main_thread_done;

    proto_sensor_data_t *data = NULL;

    while (1) {
        pthread_mutex_lock(&queue->mutex);  // Lock before checking queue size

        uint32_t number_of_elements = ucdlwb_get_number_of_elements(queue->elements);
    
        if (atomic_load_explicit(main_thread_done, memory_order_acquire) && number_of_elements == 0) {
            pthread_mutex_unlock(&queue->mutex);
            break;  // Exit when main thread is done and queue is empty
        }
    
        data = queue_remove_thread_safe_with_condition_no_lock(queue, main_thread_done); // Remove element while holding lock
        pthread_mutex_unlock(&queue->mutex);  // Unlock after removing data

        if (data == NULL) {
            continue;
        }
    
        if (params->logs_files_flag) {
            if (log_server_sensor_data(server_logs_file, data, id) == STATUS_ERROR) {
                fprintf(stderr, "Could not log sensor data\n");
                free(data);
                data = NULL;
                continue;
            }
        }
    
        (params->counter)++;
        (params->sum) += get_float_value(data);
    
        free(data);
        data = NULL;   
    }

    return NULL;
}

void wait_until_ready(int tid, pthread_mutex_t *lock, pthread_cond_t *cond, int *p_ready) {
    pthread_mutex_lock(lock);
    
    printf("Thread %d is checking if everything is ready...\n", tid);

    while (!(*p_ready)) {  // Wait until ready is set
        pthread_cond_wait(cond, lock);
    }

    printf("Everything is ready! Thread %d will start working\n", tid);
    
    pthread_mutex_unlock(lock);
}

void wake_threads(pthread_mutex_t *lock, pthread_cond_t *cond, int *p_ready) {
    pthread_mutex_lock(lock);
    *p_ready = 1;
    pthread_cond_broadcast(cond);
    pthread_mutex_unlock(lock);
}

int init_signal_handlers(struct sigaction *sa) {
    if (sa == NULL) {
        fprintf(stderr, "Invalid signal action structure pointer\n");
        return STATUS_ERROR;
    }

    sa->sa_handler = signal_handler;
    sigemptyset(&sa->sa_mask);
    sa->sa_flags = 0;
    //sa->sa_flags |= SA_RESTART; // Optionally, uncomment this line if you want to enable automatic restart of interrupted system calls

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
    /*
    void *array[10];
    size_t size;
    
    size = backtrace(array, 10);
	
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    */
    keep_running = 0;	
	
	errno = aux;   
}

// Path: src/srv/main.c