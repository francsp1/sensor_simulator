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
 * This enum defines the possible return values of this file (server_socket.c) functions.
 * @brief Status codes for the server socket functions
 * 
 * @param SERVER_SOCKET_SUCCESS The execution of the function was successful
 * 
 * @param INIT_SERVER_SOCKET_SOCKET_ERROR An error occurred while creating the server socket in the init_server_socket function
 * @param INIT_SERVER_SOCKET_REUSEADDR_ERROR An error occurred while setting the SO_REUSEADDR option in the init_server_socket function
 * @param INIT_SERVER_SOCKET_RCVBUF_ERROR An error occurred while setting the SO_RCVBUF option in the init_server_socket function
 * @param INIT_SERVER_SOCKET_BIND_ERROR An error occurred while binding the server socket in the init_server_socket function
 * @param INIT_SERVER_SOCKET_SETSOCKOPT_ERROR An error occurred while setting a socket option in the init_server_socket function
 *
 * @param RECEIVE_FROM_SOCKET_INTERUPTED_BY_SIGNAL The recvfrom function was interrupted by a signal in the receive_from_socket function
 * @param RECEIVE_FROM_SOCKET_RECVFROM_ERROR An error occurred while receiving data from the client in the receive_from_socket function
 * @param RECEIVE_FROM_SOCKET_EMPTY_PACKET An empty (size 0) UDP packet was received in the receive_from_socket 
 * @param RECEIVE_FROM_SOCKET_TRUNCATED_PACKET A truncated UDP packet was received (size smaller than sizeof(proto_sensor_data_t)) in the receive_from_socket function
 * 
 * @param DESERIALIZE_SENSOR_DATA_NULL_POINTER A null pointer was passed to the deserialize_sensor_data function
 * 
 * @typedef server_socket_status_e
 */
typedef enum server_socket_status {
    SERVER_SOCKET_SUCCESS = 0,

    INIT_SERVER_SOCKET_SOCKET_ERROR = -101,
    INIT_SERVER_SOCKET_REUSEADDR_ERROR = -102,
    INIT_SERVER_SOCKET_RCVBUF_ERROR = -103,
    INIT_SERVER_SOCKET_BIND_ERROR = -104,
    INIT_SERVER_SOCKET_SETSOCKOPT_ERROR = -105,

    RECEIVE_FROM_SOCKET_INTERUPTED_BY_SIGNAL = -201,
    RECEIVE_FROM_SOCKET_RECVFROM_ERROR = -202,
    RECEIVE_FROM_SOCKET_EMPTY_PACKET = -203,
    RECEIVE_FROM_SOCKET_TRUNCATED_PACKET = -204,

    DESERIALIZE_SENSOR_DATA_NULL_POINTER = -301,
} server_socket_status_e;

/**
 * This function initializes the server socket
 * @brief Initialize the server socket
 * @param server_port Port where the server will be listening
 * @param p_server_socket_out Pointer to the server socket file descriptor.
 * @return server_socket_status_e enum value indicating the result of the operation
 */
server_socket_status_e init_server_socket(int server_port, int *p_server_socket_out);

/**
 * This function receives data from the server socket
 * @brief Receive data from the server socket
 * @param server_socket Server socket file descriptor
 * @param buffer Pointer to the buffer where the data will be stored
 * @return server_socket_status_e enum value indicating the result of the operation
 */
server_socket_status_e receive_from_socket(int server_socket, uint8_t *buffer);

/**
 * This function deserializes the sensor data received from the client. Data is converted to host endianness
 * @brief Deserialize sensor data
 * @param buffer Pointer to the buffer where the data is stored in network endianness
 * @param p_data_out Pointer to the proto_sensor_data_t structure where the deserialized data will be stored in host endianness
 * @return server_socket_status_e enum value indicating the result of the operation
 */
server_socket_status_e deserialize_sensor_data(const uint8_t *buffer, proto_sensor_data_t *p_data_out);

#endif // _SERVER_SOCKET_H_

// Path: src/srv/server_socket.c