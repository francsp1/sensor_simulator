#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "common.h"
#include "threads.h"

int init_threads(pthread_t **tids, thread_params_t **thread_params, int server_socket){
    printf("Initializing threads\n");

    *tids = calloc(NUMBER_OF_SENSORS, sizeof(pthread_t)); 
    if (*tids == NULL){
        fprintf(stderr, "Error allocating memory for tids\n");
        return STATUS_ERROR;
    }

    *thread_params = calloc(NUMBER_OF_SENSORS, sizeof(thread_params_t));
    if (*thread_params == NULL){
        fprintf(stderr, "Error allocating memory for thread_params\n");
        free(*tids);
        return STATUS_ERROR;
    }

    for (int i = 0; i < NUMBER_OF_SENSORS; i++){
        ((*thread_params)[i]).id = i + 1;
        ((*thread_params)[i]).server_socket = server_socket;
	}

    
    for (int i = 0; i < NUMBER_OF_SENSORS; i++){
        printf("Creating thread %d\n", i);
		if ((pthread_create(&((*tids)[i]), NULL, handle_client, &((*thread_params)[i]) )) != 0) {
            fprintf(stderr, "Error creating thread\n");
            free(*tids);
            free(*thread_params);
            close(server_socket);
            return STATUS_ERROR;
        }
	}

    return STATUS_SUCCESS;
}

void *handle_client(void *arg){

    thread_params_t *params = (thread_params_t *) arg;

    printf("Thread %d\n", params->id);

    return NULL;
}