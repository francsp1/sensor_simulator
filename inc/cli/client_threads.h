#ifndef _CLIENT_THREADS_H
#define _CLIENT_THREADS_H

#include <stdint.h>
#include <pthread.h>

typedef struct client_thread_params {
	uint32_t id;
    int client_socket;
}client_thread_params_t;

int init_client_threads(pthread_t *tids, client_thread_params_t *thread_params, int client_socket, void *(*handle_client) (void *));

#endif // _CLIENT_THREADS_H