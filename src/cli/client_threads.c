/**
 * @file client_threads.c
 * @brief Implementation of the client threads
 * 
 * This file contains the functions needed to handle client threads
 *  
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path src/cli/client_threads.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include "client_threads.h"

#include "common.h"

int init_client_threads(pthread_t *tids, client_thread_params_t *thread_params, int client_socket, struct sockaddr_in *server_endpoint, logs_file_t  *client_logs_files, int packets_per_thread,void *(*handle_client) (void *)){
    printf("Initializing threads\n");

    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++){
        thread_params[i].server_endpoint = server_endpoint;
        thread_params[i].id = i;
        thread_params[i].client_socket = client_socket;
        thread_params[i].client_logs_file = &client_logs_files[i];
        thread_params[i].packets_per_sensor = packets_per_thread;
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

// Path: src/cli/client_threads.c