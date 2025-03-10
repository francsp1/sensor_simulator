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

int init_server_socket(int server_port, int *p_server_socket_out){

    printf("Initializing server socket...\n");
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        fprintf(stderr, "Error creating server socket\n");
        return STATUS_ERROR;
    }

    // Reuse port 
    int option = 1;	
	if( setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) //<-- usar antes do bind 
    {
        fprintf(stderr, "setsockopt failed!\n");
        close(server_socket);
        return STATUS_ERROR;
    }

    // Set up the server address struct
    struct sockaddr_in server_address; memset(&server_address, 0, sizeof(struct sockaddr_in));  	
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // All network interfaces
    server_address.sin_port = htons(server_port); // Server Port
    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        fprintf(stderr, "Error binding socket\n");
        close(server_socket);
        return STATUS_ERROR;
    }
    
    *p_server_socket_out = server_socket;

    printf("Server socket initialized\n");
    
    return STATUS_SUCCESS;
}

int receive_from_socket(int server_socket, uint8_t *buffer){
    socklen_t client_endpoint_length = sizeof(struct sockaddr_in);
    struct sockaddr_in client_endpoint; memset(&client_endpoint, 0, sizeof(struct sockaddr_in));
    //ssize_t read_bytes;

    //printf("Waiting for a client message...\n");

    ssize_t bytes_read = recvfrom(server_socket, buffer, ((sizeof(uint8_t) * MAX_BUFFER_SIZE) - 1) , 0, (struct sockaddr *) &client_endpoint, &client_endpoint_length);
    //printf("Received %ld bytes from %s:%d\n", bytes_read, inet_ntoa(client_endpoint.sin_addr), ntohs(client_endpoint.sin_port));
    if (bytes_read == -1) {
        if (errno == EINTR) { // Interrupted by a signal 
            fprintf(stderr, "recvfrom() interrupted by a signal\n");
            return STATUS_ERROR;
        }
        fprintf(stderr, "Error receiving data from client\n");
        return STATUS_ERROR;
    }

    //printf("Received %ld bytes from %s:%d\n", read_bytes, inet_ntoa(client_endpoint.sin_addr), ntohs(client_endpoint.sin_port));

    return STATUS_SUCCESS;
}

int deserialize_sensor_data(uint8_t *buffer, proto_sensor_data_t *p_data_out) {

    proto_sensor_data_t *data = (proto_sensor_data_t *) buffer;
    data->hdr.type = ntohl(data->hdr.type);
    data->hdr.sensor_id = ntohl(data->hdr.sensor_id);
    data->hdr.len = ntohs(data->hdr.len);
    data->data = ntohl(data->data);

    *p_data_out = *data;

    return STATUS_SUCCESS;
}

// Path: src/srv/server_socket.c