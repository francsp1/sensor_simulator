#ifndef _SERVER_SOCKET_H_
#define _SERVER_SOCKET_H_

int init_server_socket(int server_port, int *p_server_socket_out);
int close_server_socket(int server_socket);

#endif // _SERVER_SOCKET_H_