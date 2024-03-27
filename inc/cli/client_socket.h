#ifndef _CLIENT_SOCKET_H_
#define _CLIENT_SOCKET_H_

#include "common.h"

int init_client_socket(const char *ip, int port, int *p_client_socket_out, struct sockaddr_in *p_server_endpoint_out);
int pack_sensor_data(proto_sensor_data_t *data, uint32_t sensor_id);
int serialize_sensor_data(proto_sensor_data_t *data, proto_sensor_data_t *serialized_data);
int send_to_socket(int client_socket, proto_sensor_data_t *data, struct sockaddr_in *server_endpoint);

#endif // _CLIENT_SOCKET_H_