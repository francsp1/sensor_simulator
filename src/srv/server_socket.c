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

#include "server_socket.h"

#include "common.h"

int init_server_socket(int server_port, int *p_server_socket_out){

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
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(struct sockaddr_in));  	
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // All network interfaces
    server_address.sin_port = htons(server_port); // Server Port
    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        fprintf(stderr, "Error binding socket\n");
        close(server_socket);
        return STATUS_ERROR;
    }

    printf("Server listening for UDP messages on port %d\n", server_port);    
    
    *p_server_socket_out = server_socket;
    
    return STATUS_SUCCESS;
}

int receive_from_socket(int server_socket, uint8_t *buffer){
    socklen_t client_endpoint_length = sizeof(struct sockaddr_in);
    struct sockaddr_in client_endpoint;
    memset(&client_endpoint, 0, sizeof(struct sockaddr_in));
    ssize_t read_bytes;

    printf("Waiting for a client message...\n");

    if ((read_bytes = recvfrom(server_socket, buffer, (sizeof(uint8_t) * MAX_BUFFER_SIZE) - 1 , 0, (struct sockaddr *) &client_endpoint, &client_endpoint_length)) == -1) {
        fprintf(stderr, "Error receiving data from client\n");
        return STATUS_ERROR;
    }
    //printf("Thread %d received %ld bytes from %s:%d - %s\n", params->id, read_bytes, inet_ntoa(client_endpoint.sin_addr), ntohs(client_endpoint.sin_port), buffer);
    printf("Received %ld bytes from %s:%d\n", read_bytes, inet_ntoa(client_endpoint.sin_addr), ntohs(client_endpoint.sin_port));

    return STATUS_SUCCESS;
}

int deserialize_data(uint8_t *buffer, proto_send_data_t *p_data_out) {

    proto_send_data_t *data = (proto_send_data_t *) buffer;
    data->hdr.type = ntohl(data->hdr.type);
    data->hdr.sensor_id = ntohl(data->hdr.sensor_id);
    data->hdr.len = ntohs(data->hdr.len);
    data->data = ntohl(data->data);
    //float number = *((float *)&(data->data));

    *p_data_out = *data;

    return STATUS_SUCCESS;
}
