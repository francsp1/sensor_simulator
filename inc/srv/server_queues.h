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
 * This enum defines the possible return values of the functions in this module (server_queues.c).
 * @brief Status codes for the server queues functions
 * 
 * @param SERVER_QUEUES_SUCCESS The execution of the function was successful
 * 
 * @param CREATE_SERVER_QUEUES_ERROR An error occurred while creating the server queues in the create_server_queues function
 * 
 * @param PRINT_SERVER_QUEUE_NULL_LIST_ERROR An error occurred because the list of the queue is NULL in the print_server_queue function
 * @param PRINT_SERVER_QUEUE_NULL_BASE_NODE_ERROR An error occurred because the base node of the list is NULL in the print_server_queue function
 * @param PRINT_SERVER_QUEUE_CORRUPTED_LIST_ERROR An error occurred because the list is corrupted in the print_server_queue function
 * 
 * @typedef server_queues_status_e
 */
typedef enum server_queues_status {
    SERVER_QUEUES_SUCCESS = 0,

    CREATE_SERVER_QUEUES_ERROR = -101,

    PRINT_SERVER_QUEUE_NULL_LIST_ERROR = -201,
    PRINT_SERVER_QUEUE_NULL_BASE_NODE_ERROR = -202,
    PRINT_SERVER_QUEUE_CORRUPTED_LIST_ERROR = -203,
    
} server_queues_status_e;

/**
 * This function creates the queues where the messages will be stored. The number of queues is equal to the number of sensors (NUMBER_OF_SENSORS macro in common.h)
 * @brief Create the queues
 * @param queues Pointer to the array of queue_thread_safe_t structures where the queues will be stored
 * @return server_queues_status_e enum value indicating the result of the operation
 */
server_queues_status_e create_server_queues(queue_thread_safe_t **queues);

/**
 * This function destroys the queues and frees the memory allocated for them
 * @brief Destroy the queues
 * @param queues Pointer to the array of queue_thread_safe_t structures where the queues are stored
 * @return server_queues_status_e enum value indicating the result of the operation
 */
server_queues_status_e destroy_server_queues(queue_thread_safe_t **queues);

/**
 * This function prints a queue. The queue must store a pointer to a protocol_packet_t structure
 * @brief Print a queue
 * @param queue Pointer to the queue_thread_safe_t structure to be printed
 * @return server_queues_status_e enum value indicating the result of the operation
 * @note This function is used for debugging purposes
 */
server_queues_status_e print_server_queue(const queue_thread_safe_t *queue);

#endif // _SERVER_QUEUES_H_

// Path: inc/srv/server_queues.h