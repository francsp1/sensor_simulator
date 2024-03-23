#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "../inc/cli/main.h"
#include "common.h"
#include "args.h"

#include "client_socket.h"


int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    disable_buffering();
    
    struct gengetopt_args_info args;
	if (cmdline_parser(argc, argv, &args)){
        perror("Command line parser error\n");
        exit(EXIT_FAILURE);
	}

    int server_port = args.port_arg;
    if (validate_port(server_port) == STATUS_ERROR) {
        fprintf(stderr, "Wrong port value inserted\n");
        exit(EXIT_FAILURE);
    }
    
    int client_socket = -1;
    socklen_t server_endpoint_length = sizeof(struct sockaddr_in);	
    struct sockaddr_in server_endpoint = {0};
    if(init_client_socket(args.ip_arg, server_port, &client_socket, &server_endpoint) == STATUS_ERROR){
        fprintf(stderr, "Error initializing socket\n");
        exit(EXIT_FAILURE);
    }

    proto_send_data_t data;
    memset(&data, 0, sizeof(proto_send_data_t));

    data.hdr.type = htonl(PROTO_SEND_DATA);
    data.hdr.sensor_id = htonl(1);
    data.hdr.len = htons(sizeof(float));
    float number = 3.1415f;
    data.data = htonl( *((uint32_t*) &number));

    ssize_t sent_bytes;
    printf("a enviar dados para o servidor... ");
	if ((sent_bytes = sendto(client_socket, &data, sizeof(proto_send_data_t), 0, (struct sockaddr *) &server_endpoint, server_endpoint_length)) == -1) {
        fprintf(stderr, "Error sending data to server\n");
        exit(EXIT_FAILURE);
    }
	printf("ok.  (%ld bytes sent)\n", sent_bytes);

    if (close_socket(client_socket) == STATUS_ERROR) {
        fprintf(stderr, "Error closing socket\n");
        exit(EXIT_FAILURE);
    }
   
    cmdline_parser_free(&args);

    return 0;
}

void main_client(void){
    printf("Main client\n");
}
