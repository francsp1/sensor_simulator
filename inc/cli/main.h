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