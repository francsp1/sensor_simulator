/**
 * @file main.h
 * @brief Main header for the server application
 * 
 * This header file contains the prototypes of the functions that is ran by the server threads and the signal handlers
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path inc/srv/main.h
 */
#ifndef _MAIN_SRV_H_
#define _MAIN_SRV_H_

/**
 * This is the function that will be executed by each thread. It will handle the client requests by removing the message (proto_send_data_t*) from the queue and log the data int the server logs file
 * @brief Handle the client received mesaages
 * @param arg Pointer to the server_thread_params_t structure that contains the thread parameters
 * @return NULL
 */ 
void *handle_client(void *arg);

void wait_until_ready(int tid, pthread_mutex_t *lock, pthread_cond_t *cond, int *ready);

void wake_threads(pthread_mutex_t *lock, pthread_cond_t *cond, int *ready);

/**
 * This function initializes the signal handlers for the server (SIGINT and SIGTERM)
 * @brief Initialize the signal handlers
 * @param sa Pointer to the sigaction structure (struct sigaction) that will be used to set the signal handlers
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int init_signal_handlers(struct sigaction *sa);

/**
 * This function run when a SIGINT or SIGTERM signal is received. It sets the term_flag to 0 so the main thread can stop receiving messages from the sensors
 * @brief Signal handler function
 * @param signum Signal number 
 * @return void
 */
void signal_handler(int signum);

#endif // _MAIN_SRV_H_

// Path: inc/srv/main.h