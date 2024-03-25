#ifndef _SERVER_LOGS_H_
#define _SERVER_LOGS_H_

#include <pthread.h>

#include "common.h"

int open_server_logs_file(logs_file_t *server_logs_file, const char *filename);
int close_server_logs_file(logs_file_t *server_logs_file);
int log_server_sensor_data(logs_file_t *server_logs_file, proto_sensor_data_t *sensor_data, uint32_t thread_id);

#endif // _SERVER_LOGS_H_