/**
 * @file server_socket.c
 * @brief This file implements the server socket functions
 * 
 * This file implements the functions to create the server socket, receive and deserialize sensor data
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path src/srv/server_socket.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include "server_socket.h"

#include "common.h"

server_socket_status_e init_server_socket(int server_port, int *p_server_socket_out){

    printf("Initializing server socket...\n");
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        fprintf(stderr, "Error creating server socket\n");
        return INIT_SERVER_SOCKET_SOCKET_ERROR;
    }

    // Reuse port 
    int option = 1;	
	if( setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) //<-- usar antes do bind 
    {
        fprintf(stderr, "setsockopt SO_REUSEADDR failed: %s\n", strerror(errno));
        close(server_socket);
        return INIT_SERVER_SOCKET_REUSEADDR_ERROR;
    }

    /*
    // Increase the receive buffer for the UDP socket
    int recvbuf = 8 * 1024 * 1024;
    if (setsockopt(server_socket, SOL_SOCKET, SO_RCVBUF, &recvbuf, sizeof(recvbuf)) < 0) {
        fprintf(stderr, "setsockopt SO_RCVBUF failed: %s\n", strerror(errno));
        close(server_socket);
        return INIT_SERVER_SOCKET_RCVBUF_ERROR;
    }

    int actual = 0;
    socklen_t len = sizeof(actual);

    if (getsockopt(server_socket, SOL_SOCKET, SO_RCVBUF, &actual, &len) < 0) {
        fprintf(stderr, "getsockopt SO_RCVBUF failed: %s\n", strerror(errno));
    } else {
        printf("Actual receive buffer size: %d bytes\n", actual);
    }
    */

    // Set up the server address struct
    struct sockaddr_in server_address; memset(&server_address, 0, sizeof(struct sockaddr_in));  	
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // All network interfaces
    server_address.sin_port = htons(server_port); // Server Port
    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        fprintf(stderr, "Error binding socket\n");
        close(server_socket);
        return INIT_SERVER_SOCKET_BIND_ERROR;
    }
    
    *p_server_socket_out = server_socket;

    printf("Server socket initialized\n");
    
    return SERVER_SOCKET_SUCCESS;
}

 server_socket_status_e receive_from_socket(int server_socket, uint8_t *buffer, size_t *p_bytes_read_out){
    socklen_t client_endpoint_length = sizeof(struct sockaddr_in);
    struct sockaddr_in client_endpoint; memset(&client_endpoint, 0, sizeof(struct sockaddr_in));

    if (buffer == NULL || p_bytes_read_out == NULL) {
        fprintf(stderr, "Invalid buffer or output parameter pointer\n");
        return RECEIVE_FROM_SOCKET_NULL_POINTER;
    }

    //printf("Waiting for a client message...\n");

    ssize_t bytes_read = recvfrom(server_socket, buffer, ((sizeof(uint8_t) * MAX_BUFFER_SIZE) - 1) , 0, (struct sockaddr *) &client_endpoint, &client_endpoint_length);
    //printf("Received %ld bytes from %s:%d\n", bytes_read, inet_ntoa(client_endpoint.sin_addr), ntohs(client_endpoint.sin_port));
    if (bytes_read < 0) {
        if (errno == EINTR) { // Interrupted by a signal 
            fprintf(stderr, "recvfrom() interrupted by a signal\n");
            return RECEIVE_FROM_SOCKET_INTERRUPTED_BY_SIGNAL;
        }
        fprintf(stderr, "Error receiving data from client\n");
        return RECEIVE_FROM_SOCKET_RECVFROM_ERROR;
    }

    if (bytes_read == 0) {
        fprintf(stderr, "Received empty UDP packet\n");
        return RECEIVE_FROM_SOCKET_EMPTY_PACKET;
    }

    if (bytes_read < (ssize_t) sizeof(proto_hdr_s)) {
        fprintf(stderr, "Received truncated UDP packet (%zu bytes), smaller than header size (%zu bytes)\n", (size_t) bytes_read, sizeof(proto_hdr_s));
        return RECEIVE_FROM_SOCKET_TRUNCATED_PACKET;
    }

    if (bytes_read == (ssize_t) sizeof(proto_hdr_s)) {
        fprintf(stderr, "Received header-only packet (%zd bytes) with no payload\n", bytes_read);
        return RECEIVE_FROM_SOCKET_NO_PAYLOAD;
    }

    if (bytes_read > (ssize_t) (sizeof(proto_hdr_s) + MAX_PAYLOAD_SIZE)) {
        fprintf(stderr, "Received UDP packet (%zu bytes) larger than maximum expected size (%zu bytes)\n", (size_t) bytes_read, sizeof(proto_hdr_s) + MAX_PAYLOAD_SIZE);
        return RECEIVE_FROM_SOCKET_OVERSIZED_PACKET;
    }

    *p_bytes_read_out = (size_t) bytes_read;

    return SERVER_SOCKET_SUCCESS;
}

server_socket_status_e deserialize_header(const uint8_t *buffer, proto_hdr_s *p_hdr_out) {
    if (buffer == NULL || p_hdr_out == NULL) {
        return DESERIALIZE_HEADER_NULL_POINTER;
    }

    memcpy(p_hdr_out, buffer, sizeof(proto_hdr_s));

    p_hdr_out->type = ntohl(p_hdr_out->type);
    p_hdr_out->sensor_id = ntohl(p_hdr_out->sensor_id);
    p_hdr_out->len = ntohs(p_hdr_out->len);

    return SERVER_SOCKET_SUCCESS;
}

server_socket_status_e validate_header(const size_t bytes_read, const proto_hdr_s *p_hdr) {
    if (p_hdr == NULL) {
        return VALIDATE_HEADER_NULL_POINTER;
    }

    if (p_hdr->type != PROTO_SENSOR_DATA) {
        fprintf(stderr, "Invalid message type: expected %u, got %u\n", PROTO_SENSOR_DATA, p_hdr->type);
        return VALIDATE_HEADER_INVALID_TYPE;
    }

    if (p_hdr->sensor_id >= NUMBER_OF_SENSORS) {
        fprintf(stderr, "Invalid sensor ID: expected 0-%u, got %u\n", NUMBER_OF_SENSORS - 1, p_hdr->sensor_id);
        return VALIDATE_HEADER_INVALID_SENSOR_ID;
    }

    uint16_t len = p_hdr->len;

    if (len == 0) {
        fprintf(stderr, "Header declares a payload length of 0 bytes\n");
        return VALIDATE_HEADER_ZERO_LENGTH;
    }

    if (len > MAX_PAYLOAD_SIZE) {
        fprintf(stderr, "Declared payload length (%u) exceeds the maximum (%zu)\n", len, MAX_PAYLOAD_SIZE);
        return VALIDATE_HEADER_INVALID_LEN;
    }

    if (bytes_read < sizeof(proto_hdr_s) + len) {
        fprintf(stderr, "Received truncated packet (%zu bytes), expected %zu\n", bytes_read, sizeof(proto_hdr_s) + len);
        return VALIDATE_HEADER_TRUNCATED_PACKET;
    }

    if (bytes_read > sizeof(proto_hdr_s) + len) {
        fprintf(stderr, "Received packet (%zu bytes) larger than expected (%zu bytes)\n", bytes_read, sizeof(proto_hdr_s) + len);
        return VALIDATE_HEADER_OVERSIZED_PACKET;
    }

    return SERVER_SOCKET_SUCCESS;
}

server_socket_status_e deserialize_sensor_data(const uint8_t *buffer, proto_sensor_data_s *p_data_out) {
    if (buffer == NULL || p_data_out == NULL) {
        return DESERIALIZE_SENSOR_DATA_NULL_POINTER;
    }

    uint16_t len = p_data_out->hdr.len;

    if (len != sizeof(float)) {
        fprintf(stderr, "Invalid payload length for sensor data: expected %zu, got %u\n", sizeof(float), len);
        return DESERIALIZE_SENSOR_DATA_UNEXPECTED_LEN;
    }

    memcpy(p_data_out->data, buffer + sizeof(proto_hdr_s), len);

    uint32_t raw_value = 0;
    memcpy(&raw_value, p_data_out->data, sizeof(uint32_t));
    raw_value = ntohl(raw_value);
    memcpy(p_data_out->data, &raw_value, sizeof(uint32_t));

    return SERVER_SOCKET_SUCCESS;
}

// Path: src/srv/server_socket.c