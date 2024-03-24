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
#include <sys/time.h>

#include "client_socket.h"
#include "common.h"

int init_client_socket(const char *ip, int port, int *p_client_socket_out, struct sockaddr_in *p_server_endpoint_out) {

	int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        fprintf(stderr, "Error creating client socket\n");
        return STATUS_ERROR;
    }
		

	struct timeval timeout;
    memset(&timeout, 0, sizeof(timeout));
	timeout.tv_sec = 10; //seconds
	timeout.tv_usec = 0; //mili-seconds
	int setsock_ret = setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	if (setsock_ret == -1) {
        fprintf(stderr, "Can not setsockopt 'SO_RCVTIMEO'");
        close_socket(client_socket);
        return STATUS_ERROR;
    }

	struct sockaddr_in server_endpoint;
	memset(&server_endpoint, 0, sizeof(struct sockaddr_in));
	server_endpoint.sin_family = AF_INET;
    int aux = inet_pton(AF_INET, ip, &server_endpoint.sin_addr.s_addr);
    if (aux == 0) {
        fprintf(stderr, "Cannot convert IP address (IPv4): Invalid Network Address\n");
        close_socket(client_socket);
        return STATUS_ERROR;
    } else if (aux == -1) {
        fprintf(stderr, "Cannot convert IP address (IPv4)\n");
        close_socket(client_socket);
        return STATUS_ERROR;
    }
	server_endpoint.sin_port = htons(port);

    printf("Client socket created\n");

    *p_client_socket_out = client_socket;

    memcpy(p_server_endpoint_out, &server_endpoint, sizeof(struct sockaddr_in));

    return STATUS_SUCCESS;
}

int serialize_data(proto_send_data_t *p_data_out){

    proto_send_data_t data;
    memset(&data, 0, sizeof(proto_send_data_t));

    data.hdr.type = htonl(PROTO_SEND_DATA);
    data.hdr.sensor_id = htonl(1);
    data.hdr.len = htons(sizeof(float));
    float number = 3.1415f;
    data.data = htonl( *((uint32_t*) &number));

    memcpy(p_data_out, &data, sizeof(proto_send_data_t));

    return STATUS_SUCCESS;

}

int send_to_socket(int client_socket, proto_send_data_t *data, struct sockaddr_in server_endpoint, socklen_t server_endpoint_length) {

    ssize_t sent_bytes;
    printf("Sending data to server...");
	if ((sent_bytes = sendto(client_socket, data, sizeof(proto_send_data_t), 0, (struct sockaddr *) &server_endpoint, server_endpoint_length)) == -1) {
        fprintf(stderr, "Error sending data to server\n");
        exit(EXIT_FAILURE);
    }
	printf("ok.  (%ld bytes sent)\n", sent_bytes);

    return STATUS_SUCCESS;
}