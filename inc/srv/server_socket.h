/**
 * @file server_socket.h
 * @brief Header file for the server socket functions
 * 
 * This header file provides the prototypes of the functions to create and manage the server socket, receive and deserialize sensor data from the client
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path inc/srv/server_socket.h
 */
#ifndef _SERVER_SOCKET_H_
#define _SERVER_SOCKET_H_

#include <stdint.h>
#include "common.h"

/**
 * This function initializes the server socket
 * @brief Initialize the server socket
 * @param server_port Port where the server will be listening
 * @param p_server_socket_out Pointer to the server socket file descriptor.
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int init_server_socket(int server_port, int *p_server_socket_out);

/**
 * This function receives data from the server socket
 * @brief Receive data from the server socket
 * @param server_socket Server socket file descriptor
 * @param buffer Pointer to the buffer where the data will be stored
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure. If the functions is terminated by a signal the errno is set to EINTR
 */
int receive_from_socket(int server_socket, uint8_t *buffer);

/**
 * This function deserializes the sensor data received from the client. Data is converted to host endianness
 * @brief Deserialize sensor data
 * @param buffer Pointer to the buffer where the data is stored in network endianness
 * @param p_data_out Pointer to the proto_sensor_data_t structure where the deserialized data will be stored in host endianness
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int deserialize_sensor_data(uint8_t *buffer, proto_sensor_data_t *p_data_out);

#endif // _SERVER_SOCKET_H_

// Path: src/srv/server_socket.c