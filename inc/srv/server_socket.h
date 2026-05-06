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
 * This enum defines the status codes returned by the init_server_socket function
 * @brief Status codes for the init_server_socket function
 * @param INIT_SERVER_SOCKET_SUCCESS The execution of the function init_server_socket was successful
 * @param INIT_SERVER_SOCKET_SOCKET_ERROR An error occurred while creating the server socket
 * @param INIT_SERVER_SOCKET_REUSEADDR_ERROR An error occurred while setting the SO_REUSE
 * @param INIT_SERVER_SOCKET_RCVBUF_ERROR An error occurred while setting the SO_RCVBUF
 * @param INIT_SERVER_SOCKET_BIND_ERROR An error occurred while binding the server socket
 * @param INIT_SERVER_SOCKET_SETSOCKOPT_ERROR An error occurred while setting a socket option
 * @typedef init_server_socket_status_e
 */
typedef enum init_server_socket_status {
    INIT_SERVER_SOCKET_SUCCESS = 0,
    INIT_SERVER_SOCKET_SOCKET_ERROR = -1,
    INIT_SERVER_SOCKET_REUSEADDR_ERROR = -2,
    INIT_SERVER_SOCKET_RCVBUF_ERROR = -3,
    INIT_SERVER_SOCKET_BIND_ERROR = -4,
    INIT_SERVER_SOCKET_SETSOCKOPT_ERROR = -5,
} init_server_socket_status_e;

/**
 * This function initializes the server socket
 * @brief Initialize the server socket
 * @param server_port Port where the server will be listening
 * @param p_server_socket_out Pointer to the server socket file descriptor.
 * @return init_server_socket_status_e enum value indicating the result of the operation
 */
init_server_socket_status_e init_server_socket(int server_port, int *p_server_socket_out);

/**
 * this enum defines the status codes returned by the receive_from_socket function
 * @brief Status codes for the receive_from_socket function
 * @param RECEIVE_FROM_SOCKET_SUCCESS The execution of the function receive_from_socket was successful
 * @param RECEIVE_FROM_SOCKET_INTERUPTED_BY_SIGNAL The execution of the function receive_from_socket was interrupted by a signal, the errno is set to EINTR
 * @param RECEIVE_FROM_SOCKET_RECVFROM_ERROR An error occurred while receiving data from the socket
 * @param RECEIVE_FROM_SOCKET_EMPTY_PACKET An empty UDP packet was received
 * @param RECEIVE_FROM_SOCKET_TRUNCATED_PACKET A truncated UDP packet was received, the number of bytes received is less than the size of proto_sensor_data
 * @typedef receive_from_socket_status_e
 */
typedef enum receive_from_socket_status {
    RECEIVE_FROM_SOCKET_SUCCESS = 0,
    RECEIVE_FROM_SOCKET_INTERUPTED_BY_SIGNAL = -1,
    RECEIVE_FROM_SOCKET_RECVFROM_ERROR = -2,
    RECEIVE_FROM_SOCKET_EMPTY_PACKET = -3,
    RECEIVE_FROM_SOCKET_TRUNCATED_PACKET = -4,
} receive_from_socket_status_e;

/**
 * This function receives data from the server socket
 * @brief Receive data from the server socket
 * @param server_socket Server socket file descriptor
 * @param buffer Pointer to the buffer where the data will be stored
 * @return receive_from_socket_status_e enum value indicating the result of the operation
 */
receive_from_socket_status_e receive_from_socket(int server_socket, uint8_t *buffer);

/**
 * This enum defines the status codes returned by the deserialize_sensor_data function
 * @brief Status codes for the deserialize_sensor_data function
 * @param DESERIALIZE_SENSOR_DATA_SUCCESS The execution of the function deserialize_sensor_data was successful
 * @param DESERIALIZE_SENSOR_DATA_NULL_POINTER A null pointer was passed as an argument to the function deserialize_sensor_data
 * @typedef deserialize_sensor_data_status_e
 */
typedef enum deserialize_sensor_data_status {
    DESERIALIZE_SENSOR_DATA_SUCCESS = 0,
    DESERIALIZE_SENSOR_DATA_NULL_POINTER = -1,
} deserialize_sensor_data_status_e;

/**
 * This function deserializes the sensor data received from the client. Data is converted to host endianness
 * @brief Deserialize sensor data
 * @param buffer Pointer to the buffer where the data is stored in network endianness
 * @param p_data_out Pointer to the proto_sensor_data_t structure where the deserialized data will be stored in host endianness
 * @return deserialize_sensor_data_status_e enum value indicating the result of the operation
 */
deserialize_sensor_data_status_e deserialize_sensor_data(const uint8_t *buffer, proto_sensor_data_t *p_data_out);

#endif // _SERVER_SOCKET_H_

// Path: src/srv/server_socket.c