#include <stdio.h>
#include <stdlib.h>

#include "../inc/srv/main.h"
#include "common.h"
#include "args.h"

#include "socket.h"


int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

	struct gengetopt_args_info args;
	if(cmdline_parser(argc, argv, &args)){
        perror("Command line parser error\n");
        exit(EXIT_FAILURE);
	}

    int server_port = args.port_arg;   
    if (validate_port(server_port) == STATUS_ERROR) {
        exit(EXIT_FAILURE);
    }

    int server_socket = -1;
    if (init_socket(server_port, &server_socket) == STATUS_ERROR) {
        fprintf(stderr, "Error initializing socket\n");
        exit(EXIT_FAILURE);
    }



	cmdline_parser_free(&args);
    return 0;
}

void main_server(void) {
    printf("Main server\n");
}
