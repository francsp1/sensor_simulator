#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "server_threads.h"

#include "common.h"

int init_threads(pthread_t *tids, thread_params_t *thread_params, int server_socket, void *(*handle_client) (void *) ){
    printf("Initializing threads\n");

    for (int i = 0; i < NUMBER_OF_SENSORS; i++){
        thread_params[i].id = i + 1;
        thread_params[i].server_socket = server_socket;
    }

    for (int i = 0; i < NUMBER_OF_SENSORS; i++){
        if ((pthread_create(&tids[i], NULL, handle_client, &thread_params[i])) != 0){
            fprintf(stderr, "Error creating thread %d\n", i);
            return STATUS_ERROR;
        }
    }

    return STATUS_SUCCESS;
}

int join_threads(pthread_t *tids){
    printf("Joining threads\n");

    for (int i = 0; i < NUMBER_OF_SENSORS; i++){
        if (pthread_join(tids[i], NULL) != 0){
            fprintf(stderr, "Error joining thread %d\n", i);
            return STATUS_ERROR;
        }
    }

    return STATUS_SUCCESS;
}