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
    //socklen_t server_endpoint_length = sizeof(struct sockaddr_in);	
    struct sockaddr_in server_endpoint = {0};
    if(init_client_socket(args.ip_arg, server_port, &client_socket, &server_endpoint) == STATUS_ERROR){
        fprintf(stderr, "Error initializing socket\n");
        exit(EXIT_FAILURE);
    }
    
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
