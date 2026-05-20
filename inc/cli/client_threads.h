/**
 * @file client_threads.h 
 * @brief Header file for the client threads
 * 
 * This header file provides the prototypes of the functions to initialize the client threads and the structure to pass the parameters to the client threads
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path inc/cli/client_threads.h
 */
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
 * @param client_logs_file Pointer to the logs_file_s stucture. The structure contains the the pointer (FILE *)for the corresponding logs file of the sensor and a pointer to a string that contains the file name (Ex.: the thread with the ID 0 will write the logs to a fila named "sensor_0_client_logs.txt")
 * @param packets_per_sensor Number of packets to be sent by each sensor
 */
typedef struct client_thread_params {
    struct sockaddr_in *server_endpoint;
	uint32_t id;
    int client_socket;
    int logs_files_flag;
    logs_file_s *client_logs_file;
    int packets_per_sensor;
} client_thread_params_s;

/**
 * This enum defines the status codes returned by the client_threads functions
 * @brief Status codes for the client_threads module
 * @param CLIENT_QUEUES_SUCCESS The execution of the function was successful
 * @param INIT_CLIENT_THREADS_PTHREAD_CREATE_ERROR pthread_create() failed while creating a thread
 * @typedef client_queues_status_e
 */
typedef enum client_queues_status {
    CLIENT_QUEUES_SUCCESS = 0,
    
    INIT_CLIENT_THREADS_PTHREAD_CREATE_ERROR = -101,
} client_queues_status_e;

/**
 * This function initializes the client threads. Thee number od threads is equal to the number of sensors (NUMBER_OF_SENSORS macro in common.h)
 * @brief Initialize the client threads
 * @param tids Pointer to the array of pthread_t structures where the thread IDs will be stored
 * @param thread_params Pointer to the array of client_thread_params_s structures where the thread parameters will be stored
 * @param client_socket Client socket to send the data
 * @param server_endpoint Pointer to the server endpoint (struct sockaddr_in) to send the data to
 * @param client_logs_file Pointer to the logs_file_s stucture. The structure contains the the pointer (FILE *)for the corresponding logs file of the sensor and a pointer to a string that contains the file name (Ex.: the thread with the ID 0 will write the logs to a fila named "sensor_0_client_logs.txt")
 * @param packets_per_sensor Number of packets to be sent by each sensor
 * @param handle_client Pointer to the function that will be executed by each thread
 * @return client_queues_status_e value indicating the result of the operation
 */
client_queues_status_e init_client_threads(pthread_t *tids, client_thread_params_s *thread_params, int client_socket, struct sockaddr_in *server_endpoint, int logs_files_flag, logs_file_s  *client_logs_files, int packets_per_thread,void *(*handle_client) (void *));

#endif // _CLIENT_THREADS_H

// Path: inc/cli/client_threads.h