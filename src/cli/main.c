#include <stdio.h>
#include <stdlib.h>

#include "../inc/cli/main.h"
#include "common.h"
#include "args.h"

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

    /*
    if (init_client_socket(server_port) == STATUS_ERROR) {
        fprintf(stderr, "Error initializing socket\n");
        exit(EXIT_FAILURE);
    }
    */
   
    cmdline_parser_free(&args);

    return 0;
}

void main_client(void){
    printf("Main client\n");
}
