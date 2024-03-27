/**
 * @file main.h
 * @brief Main header for the client application
 * 
 * this header file contains the prototypes of the function tis ran by the client threads
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path inc/cli/main.h
 */
#ifndef _MAIN_CLI_H_
#define _MAIN_CLI_H_

/**
 * This is the fucntion that will be executed by the client sensor threads. It packs, serializes and sends the sensor data to the server.
 * @brief Handle the client threads
 * @param arg Pointer to the client_thread_params_t structure that contains the parameters of the thread
 * @return NULL
 */
void *handle_server(void *arg);

#endif // _MAIN_CLI_H_

// Path: inc/cli/main.h