#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int close_socket(int server_socket){
    if (close(server_socket) == -1) {
        fprintf(stderr, "Error closing socket\n");
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

float get_float_value(proto_send_data_t *data){
    return *((float *)&(data->data));
}

void common(void){
    printf("Common\n");
}

