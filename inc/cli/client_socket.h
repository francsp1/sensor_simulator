#ifndef _CLIENT_SOCKET_H_
#define _CLIENT_SOCKET_H_

#include "common.h"

int init_client_socket(const char *ip, int port, int *p_client_socket_out, struct sockaddr_in *p_server_endpoint_out);
int serialize_data(proto_send_data_t *data);
int send_to_socket(int client_socket, proto_send_data_t *data, struct sockaddr_in server_endpoint, socklen_t server_endpoint_length);

#endif // _CLIENT_SOCKET_H_