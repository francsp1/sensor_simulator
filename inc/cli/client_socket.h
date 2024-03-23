#ifndef _CLIENT_SOCKET_H_
#define _CLIENT_SOCKET_H_

int init_client_socket(const char *ip, int port, int *p_client_socket_out, struct sockaddr_in *p_server_endpoint_out);

#endif // _CLIENT_SOCKET_H_