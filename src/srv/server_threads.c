/**
 * @file server_threads.c
 * @brief Implementation of the server threads
 * 
 * This file contains the functions needed to handle server threads
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path src/srv/server_threads.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include "server_threads.h"

#include "common.h"

int init_server_threads(pthread_t *tids, server_thread_params_t *thread_params, int server_socket, logs_file_t *server_logs_file, queue_thread_safe_t **queues, void *(*handle_client) (void *)) {
    printf("Initializing threads...\n");

    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++){
        thread_params[i].id = i;
        thread_params[i].server_socket = server_socket;
        thread_params[i].counter = 0;
        thread_params[i].sum = 0;
        thread_params[i].queue = queues[i];
        thread_params[i].server_logs_file = server_logs_file;
    }

    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++){
        if ((pthread_create(&tids[i], NULL, handle_client, &thread_params[i])) != 0){
            fprintf(stderr, "Error creating thread %d\n", i);
            return STATUS_ERROR;
        }
    }

    printf("Threads initialized\n");
    return STATUS_SUCCESS;
}

// Path: src/srv/server_threads.c