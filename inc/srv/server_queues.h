/**
 * @file server_queues.h
 * @brief Header file for the server queues
 * 
 * This header file provides the prototypes of the functions to create and destroy the server queues
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path inc/srv/server_queues.h
 */
#ifndef _SERVER_QUEUES_H_
#define _SERVER_QUEUES_H_

#include "queue_thread_safe.h"

/**
 * This function creates the queues where the messages will be stored. The number of queues is equal to the number of sensors (NUMBER_OF_SENSORS macro in common.h)
 * @brief Create the queues
 * @param queues Pointer to the array of queue_thread_safe_t structures where the queues will be stored
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int create_queues(queue_thread_safe_t **queues);

/**
 * This function destroys n queues and frees the memory allocated for them
 * @brief Destroy n queues
 * @param queues Pointer to the array of queue_thread_safe_t structures where the queues are stored
 * @param n Number of queues to be destroyed
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 * @note This function should only be used internally. Do not use it directly
 */
int _destroy_n_queues(queue_thread_safe_t **queues, uint32_t n);

/**
 * This function destroys the queues and frees the memory allocated for them
 * @brief Destroy the queues
 * @param queues Pointer to the array of queue_thread_safe_t structures where the queues are stored
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int destroy_queues(queue_thread_safe_t **queues);

/**
 * This function prints a queue. The queue must store a pointer to a protocol_packet_t structure
 * @brief Print a queue
 * @param queue Pointer to the queue_thread_safe_t structure to be printed
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 * @note This function is used for debugging purposes
 */
int print_queue(const queue_thread_safe_t *queue);

#endif // _SERVER_QUEUES_H_

// Path: inc/srv/server_queues.h