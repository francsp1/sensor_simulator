#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "common.h"
#include "threads.h"

int init_threads(pthread_t **tids, thread_params_t **thread_params, int server_socket, void *(*handle_client) (void *) ){
    printf("Initializing threads\n");

    *tids = calloc(NUMBER_OF_SENSORS, sizeof(pthread_t)); 
    if (*tids == NULL){
        fprintf(stderr, "Error allocating memory for tids\n");
        return STATUS_ERROR;
    }

    *thread_params = calloc(NUMBER_OF_SENSORS, sizeof(thread_params_t));
    if (*thread_params == NULL){
        fprintf(stderr, "Error allocating memory for thread_params\n");
        free_tids(tids);
        return STATUS_ERROR;
    }

    for (int i = 0; i < NUMBER_OF_SENSORS; i++){
        ((*thread_params)[i]).id = i + 1;
        ((*thread_params)[i]).server_socket = server_socket;
	}

    
    for (int i = 0; i < NUMBER_OF_SENSORS; i++){
		if ((pthread_create(&((*tids)[i]), NULL, handle_client, &((*thread_params)[i]) )) != 0){
            fprintf(stderr, "Error creating thread %d\n", i);
            free_threads(tids, thread_params);
            return STATUS_ERROR;
        }
	}

    return STATUS_SUCCESS;
}

void free_tids(pthread_t **tids){
    free(*tids);
    *tids = NULL;
}

void free_thread_params(thread_params_t **thread_params){
    free(*thread_params);
    *thread_params = NULL;
}

void free_threads(pthread_t **tids, thread_params_t **thread_params){
    free_tids(tids);
    free_thread_params(thread_params);
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