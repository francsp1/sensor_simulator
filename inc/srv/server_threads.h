#ifndef _THREADS_H
#define _THREADS_H

#include <stdint.h>
#include <pthread.h>

#include "queue_thread_safe.h"

#include "common.h"


/**
 * This structure contains the parameters that will be passed to the server threads
 * @brief Server thread parameters
 * @param id Thread ID
 * @param server_socket Server socket file descriptor
 * @param counter Counter of the number of packets received
 * @param sum Sum of the values of the packets received
 * @param queue Pointer to the queue where the packets will be stored
 * @param server_logs_file Pointer to the logs_file_t structure where the logs will be stored. logs_file_t contains the file descriptor of the logs file and a mutex to protect the file descriptor
 * @struct server_thread_params_t
 */
typedef struct server_thread_params {
	uint32_t id;
    int server_socket;
    uint32_t counter;
    float sum;
    queue_thread_safe_t * queue;
    logs_file_t *server_logs_file;
}server_thread_params_t;

/**
 * This function initializes the server threads. The number of threads is equal to the number of sensors (NUMBER_OF_SENSORS macro in common.h)
 * @brief Initialize the server threads
 * @param tids Pointer to the array of pthread_t structures where the thread IDs will be stored
 * @param thread_params Pointer to the array of server_thread_params_t structures where the thread parameters will be stored
 * @param server_socket Server socket file descriptor
 * @param server_logs_file Pointer to the logs_file_t structure where the logs will be stored. logs_file_t contains the file descriptor of the logs file and a mutex to protect the file descriptor
 * @param queues Pointer to the array of queue_thread_safe_t structures where the queues will be stored
 * @param handle_client Pointer to the function that will be executed by each thread
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int init_server_threads(pthread_t *tids, server_thread_params_t *thread_params, int server_socket, logs_file_t *server_logs_file, queue_thread_safe_t **queues, void *(*handle_client) (void *));

#endif  // _THREADS_H