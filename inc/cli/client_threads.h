#ifndef _CLIENT_THREADS_H
#define _CLIENT_THREADS_H

#include <stdint.h>
#include <pthread.h>

#include "common.h"

typedef struct client_thread_params {
    struct sockaddr_in *server_endpoint;
	uint32_t id;
    int client_socket;
    logs_file_t *client_logs_file;
    int packets_per_sensor;
}client_thread_params_t;

int init_client_threads(pthread_t *tids, client_thread_params_t *thread_params, int client_socket, struct sockaddr_in *server_endpoint, logs_file_t  *client_logs_file, int packets_per_sensor, void *(*handle_client) (void *));

#endif // _CLIENT_THREADS_H