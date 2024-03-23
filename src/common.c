#include <stdio.h>
#include <stdlib.h>

#include "common.h"

int validate_port(int server_port) {
    if (server_port < 1024 || server_port > 65535) {
        fprintf(stderr, "Invalid port number. Should be between 1024 and 65535.\n");
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

void disable_buffering(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

void common(void){
    printf("Common\n");
}

