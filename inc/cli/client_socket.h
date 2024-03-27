/**
 * @file client_socket.h
 * @brief Header file for the client socket functions
 * 
 * This header file provides the prototypes of the functions to create and manage the client socket, pack, serialize and send sensor data to the server
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path inc/cli/client_socket.h
 */
#ifndef _CLIENT_SOCKET_H_
#define _CLIENT_SOCKET_H_

#include "common.h"

/**
 * This function initializes the socket to send UDP sensor messages to the server
 * @brief Initialize the sensor simulation (client) socket
 * @param ip IP address string of the server to connect to
 * @param port Port number of the server to connect to
 * @param p_client_socket_out Pointer to the client socket to be initialized
 * @param p_server_endpoint_out Pointer to the server endpoint (struct sockaddr_in) to be initialized
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int init_client_socket(const char *ip, int port, int *p_client_socket_out, struct sockaddr_in *p_server_endpoint_out);

/**
 * This function packs the data to be sent to the server in the proto_sensor_data_t structure
 * @brief Pack sensor data to the server
 * @param data Pointer to the proto_sensor_data_t structure where the data will be packed
 * @param sensor_id ID of the sensor that sends the data
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
  
 */
int pack_sensor_data(proto_sensor_data_t *data, uint32_t sensor_id);

/**
 * This function serializes the sensor data to be sent to the server. Data is converted to network endianness
 * @brief Serialize sensor data
 * @param data Pointer to the proto_sensor_data_t structure where the data original data is stored in host endianness
 * @param serialized_data Pointer to the proto_sensor_data_t structure where the serialized data will be stored in network endianness
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int serialize_sensor_data(proto_sensor_data_t *data, proto_sensor_data_t *serialized_data);

/**
 * This function sends the serialized sensor data to the server
 * @brief Send sensor data to the server
 * @param client_socket Client socket to send the data
 * @param data Pointer to the proto_sensor_data_t structure where the serialized data to be sent is stored
 * @param server_endpoint Pointer to the server endpoint (struct sockaddr_in) to send the data to
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int send_to_socket(int client_socket, proto_sensor_data_t *data, struct sockaddr_in *server_endpoint);

#endif // _CLIENT_SOCKET_H_

// Path: inc/cli/client_socket.h