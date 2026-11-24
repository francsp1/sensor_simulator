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
 * This enum defines the possible return values of the functions in this module (server_socket.c).
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
 * @param RECEIVE_FROM_SOCKET_NULL_POINTER An error occurred because a null pointer was passed to the receive_from_socket function
 * @param RECEIVE_FROM_SOCKET_INTERRUPTED_BY_SIGNAL An error occurred because the recvfrom function was interrupted by a signal in the receive_from_socket function
 * @param RECEIVE_FROM_SOCKET_RECVFROM_ERROR An error occurred while receiving data from the client in the receive_from_socket function
 * @param RECEIVE_FROM_SOCKET_EMPTY_PACKET An error occurred because an empty (size 0) UDP packet was received in the receive_from_socket function
 * @param RECEIVE_FROM_SOCKET_TRUNCATED_PACKET An error occurred because a truncated UDP packet was received (size smaller than the message header) in the receive_from_socket function
 * @param RECEIVE_FROM_SOCKET_NO_PAYLOAD An error occurred because a header-only UDP packet (no payload bytes) was received in the receive_from_socket function
 * @param RECEIVE_FROM_SOCKET_OVERSIZED_PACKET An error occurred because a UDP packet larger than the header plus MAX_PAYLOAD_SIZE was received in the receive_from_socket function
 *
 * @param DESERIALIZE_HEADER_NULL_POINTER An error occurred because a null pointer was passed to the deserialize_header function
 * @param DESERIALIZE_HEADER_TRUNCATED_BUFFER An error occurred because the number of bytes actually received from the socket was smaller than the number of bytes required to deserialize the message header
 * 
 * @param VALIDATE_HEADER_NULL_POINTER An error occurred because a null pointer was passed to the validate_header function
 * @param VALIDATE_HEADER_INVALID_TYPE An error occurred because the header declared a message type other than PROTO_SENSOR_DATA in the validate_header function
 * @param VALIDATE_HEADER_INVALID_SENSOR_ID An error occurred because the header declared a sensor ID outside the range [0, NUMBER_OF_SENSORS) in the validate_header function
 * @param VALIDATE_HEADER_ZERO_LENGTH An error occurred because the header declared a payload length of 0 bytes in the validate_header function
 * @param VALIDATE_HEADER_INVALID_LEN An error occurred because the header declared a payload length larger than MAX_PAYLOAD_SIZE in the validate_header function
 * @param VALIDATE_HEADER_TRUNCATED_PACKET An error occurred because the received packet was smaller than the header plus the declared payload length in the validate_header function
 * @param VALIDATE_HEADER_OVERSIZED_PACKET An error occurred because the received packet was larger than the header plus the declared payload length in the validate_header function
 *
 * @param DESERIALIZE_PAYLOAD_NULL_POINTER An error occurred because a null pointer was passed to the deserialize_payload function
 * @param DESERIALIZE_PAYLOAD_UNEXPECTED_LEN An error occurred because the header declared a payload length that is not the expected size for sensor data in the deserialize_payload function
 * @param DESERIALIZE_PAYLOAD_TRUNCATED_BUFFER An error occurred because the number of bytes actually received from the socket was smaller than the number of bytes required to deserialize the sensor data payload
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

    RECEIVE_FROM_SOCKET_NULL_POINTER = -201,
    RECEIVE_FROM_SOCKET_INTERRUPTED_BY_SIGNAL = -202,
    RECEIVE_FROM_SOCKET_RECVFROM_ERROR = -203,
    RECEIVE_FROM_SOCKET_EMPTY_PACKET = -204,
    RECEIVE_FROM_SOCKET_TRUNCATED_PACKET = -205,
    RECEIVE_FROM_SOCKET_NO_PAYLOAD = -206,
    RECEIVE_FROM_SOCKET_OVERSIZED_PACKET = -207,

    DESERIALIZE_HEADER_NULL_POINTER = -301,
    DESERIALIZE_HEADER_TRUNCATED_BUFFER = -302,

    VALIDATE_HEADER_NULL_POINTER = -401,
    VALIDATE_HEADER_INVALID_TYPE = -402,
    VALIDATE_HEADER_INVALID_SENSOR_ID = -403,
    VALIDATE_HEADER_ZERO_LENGTH = -404,
    VALIDATE_HEADER_INVALID_LEN = -405,
    VALIDATE_HEADER_TRUNCATED_PACKET = -406,
    VALIDATE_HEADER_OVERSIZED_PACKET = -407,

    DESERIALIZE_PAYLOAD_NULL_POINTER = -501,
    DESERIALIZE_PAYLOAD_UNEXPECTED_LEN = -502,
    DESERIALIZE_PAYLOAD_TRUNCATED_BUFFER = -503,
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
 * @param p_bytes_read_out Pointer where the number of bytes read from the socket will be stored
 * @return server_socket_status_e enum value indicating the result of the operation
 */
server_socket_status_e receive_from_socket(int server_socket, uint8_t *buffer, size_t *p_bytes_read_out);

/**
 * This function deserializes the message header received from the client.
 * It verifies that the received message contains a complete header and
 * converts the header fields from network byte order to host byte order.
 *
 * @brief Deserialize the message header.
 *
 * @param buffer Pointer to the received message in network byte order.
 * @param bytes_read Number of bytes actually received by recvfrom().
 * @param p_hdr_out Pointer where the deserialized header will be stored in host byte order.
 *
 * @return SERVER_SOCKET_SUCCESS on success.
 * @return DESERIALIZE_HEADER_NULL_POINTER if buffer or p_hdr_out is NULL.
 * @return DESERIALIZE_HEADER_TRUNCATED_BUFFER if bytes_read is smaller than
 *         sizeof(proto_hdr_s).
 */
server_socket_status_e deserialize_header(const uint8_t *buffer, size_t bytes_read, proto_hdr_s *p_hdr_out);

/**
 * This function validates a deserialized message header. The header must already be in host endianness (see deserialize_header)
 * @brief Validate the message header
 * @param bytes_read Number of bytes received in the buffer, used to validate the declared payload length
 * @param p_hdr Pointer to the deserialized proto_hdr_s structure to validate
 * @return server_socket_status_e enum value indicating the result of the operation
 */
server_socket_status_e validate_header(const size_t bytes_read, const proto_hdr_s *p_hdr);

/**
 * This function deserializes the sensor data payload received from the client.
 * It verifies that the received message contains the complete declared payload
 * and converts the payload from network byte order to host byte order.
 * @brief Deserialize sensor data payload
 * @note The header must already be deserialized and validated (see deserialize_header) before calling this function
 * @param buffer Pointer to the buffer where the message is stored in network endianness
 * @param bytes_read Number of bytes actually received by recvfrom()
 * @param p_data_out Pointer to the proto_sensor_data_s structure with the deserialized header, where the payload will be stored in host endianness
 * @return SERVER_SOCKET_SUCCESS on success
 * @return DESERIALIZE_PAYLOAD_NULL_POINTER if buffer or p_data_out is NULL
 * @return DESERIALIZE_PAYLOAD_UNEXPECTED_LEN if the declared payload length is not the expected sensor data size
 * @return DESERIALIZE_PAYLOAD_TRUNCATED_BUFFER if bytes_read is smaller than the header plus the declared payload length
 */
server_socket_status_e deserialize_payload(const uint8_t *buffer, size_t bytes_read, proto_sensor_data_s *p_data_out);

#endif // _SERVER_SOCKET_H_

// Path: src/srv/server_socket.c
