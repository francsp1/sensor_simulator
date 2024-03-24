#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include "client_threads.h"

#include "common.h"

int init_client_threads(pthread_t *tids, client_thread_params_t *thread_params, int client_socket, void *(*handle_client) (void *)){
    printf("Initializing threads\n");

    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++){
        thread_params[i].id = i;
        thread_params[i].client_socket = client_socket;
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
