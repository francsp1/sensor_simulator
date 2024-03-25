#ifndef _THREADS_H
#define _THREADS_H

#include <stdint.h>
#include <pthread.h>

#include "queue_thread_safe.h"

#include "common.h"

typedef struct server_thread_params {
	uint32_t id;
    int server_socket;
    queue_thread_safe_t * queue;
    logs_file_t *server_logs_file;
}server_thread_params_t;

int init_server_threads(pthread_t *tids, server_thread_params_t *thread_params, int server_socket, logs_file_t *server_logs_file, queue_thread_safe_t **queues, void *(*handle_client) (void *));
int join_threads(pthread_t *tids);

#endif  // _THREADS_H