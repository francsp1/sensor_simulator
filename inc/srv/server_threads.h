#ifndef _THREADS_H
#define _THREADS_H

#include <pthread.h>

typedef struct thread_params {
	int id;
    int server_socket;
}thread_params_t;

int init_threads(pthread_t *tids, thread_params_t *thread_params, int server_socket, void *(*handle_client) (void *) );
int join_threads(pthread_t *tids);


#endif  // _THREADS_H