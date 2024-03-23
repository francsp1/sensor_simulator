#ifndef _SOCKET_H_
#define _SOCKET_H_

int init_server_socket(int server_port, int *p_server_socket_out);
int close_server_socket(int server_socket);

#endif // _SOCKET_H_