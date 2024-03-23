#ifndef _SERVER_SOCKET_H_
#define _SERVER_SOCKET_H_

#include <stdint.h>
#include "common.h"

int init_server_socket(int server_port, int *p_server_socket_out);
int receive_from_socket(int server_socket, uint8_t *buffer);
int deserialize_data(uint8_t *buffer, proto_send_data_t *p_data_out);

#endif // _SERVER_SOCKET_H_