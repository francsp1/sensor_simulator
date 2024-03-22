#ifndef _SOCKET_H_
#define _SOCKET_H_

int init_socket(int server_port, int *server_socket_out);
int close_socket(int server_socket);

#endif // _SOCKET_H_