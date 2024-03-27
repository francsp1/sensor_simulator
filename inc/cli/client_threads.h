#ifndef _CLIENT_THREADS_H
#define _CLIENT_THREADS_H

#include <stdint.h>
#include <pthread.h>

#include "common.h"

/**
 * This structure contains the parameters that will be passed to the client sensor threads
 * @brief Client thread parameters
 * @param server_endpoint Pointer to the server endpoint (struct sockaddr_in) to send the data to
 * @param id Thread/Sensor ID
 * @param client_socket Client socket to send the data
 * @param client_logs_file Pointer to the logs_file_t structure where the logs will be stored. logs_file_t contains the file descriptor of the logs file and a mutex to protect the file descriptor
 * @param packets_per_sensor Number of packets to be sent by each sensor
 */
typedef struct client_thread_params {
    struct sockaddr_in *server_endpoint;
	uint32_t id;
    int client_socket;
    logs_file_t *client_logs_file;
    int packets_per_sensor;
}client_thread_params_t;

/**
 * This function initializes the client threads. Thee number od threads is equal to the number of sensors (NUMBER_OF_SENSORS macro in common.h)
 * @brief Initialize the client threads
 * @param tids Pointer to the array of pthread_t structures where the thread IDs will be stored
 * @param thread_params Pointer to the array of client_thread_params_t structures where the thread parameters will be stored
 * @param client_socket Client socket to send the data
 * @param server_endpoint Pointer to the server endpoint (struct sockaddr_in) to send the data to
 * @param client_logs_file Pointer to the logs_file_t structure where the logs will be stored. logs_file_t contains the file descriptor of the logs file and a mutex to protect the file descriptor
 * @param packets_per_sensor Number of packets to be sent by each sensor
 * @param handle_client Pointer to the function that will be executed by each thread
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int init_client_threads(pthread_t *tids, client_thread_params_t *thread_params, int client_socket, struct sockaddr_in *server_endpoint, logs_file_t  *client_logs_file, int packets_per_sensor, void *(*handle_client) (void *));

#endif // _CLIENT_THREADS_H