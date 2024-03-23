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
