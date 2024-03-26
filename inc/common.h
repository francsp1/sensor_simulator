#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <pthread.h>

#define STATUS_ERROR   ( (int) (-1) )
#define STATUS_SUCCESS ( (int) (0)  )

#define MAX_PENDING_CONECTIONS (2048)
#define MAX_BUFFER_SIZE (1024)

#define NUMBER_OF_SENSORS ( (uint32_t) (10) )

#define SENSOR_RATE (10) // Define the desired frequency in Hz
#define DELAY_MS (1000 / SENSOR_RATE) // Calculate delay in milliseconds

#define TIME_BUFFER_SIZE (20) 

typedef enum {
	PROTO_SENSOR_DATA,
} proto_type_e;

typedef struct {
	proto_type_e type;
    uint32_t sensor_id;
    uint16_t len;
} proto_hdr_t;

typedef struct {
    proto_hdr_t hdr;
    uint32_t data;
} proto_sensor_data_t;

int validate_port(int server_port);
void disable_buffering(void);
int close_socket(int server_socket);
float get_float_value(proto_sensor_data_t *data);
int get_current_time(char **buffer);

int join_threads(pthread_t *tids);


#define SERVER_LOGS_FILE "server_logs.txt"
#define CLIENT_LOGS_FILE "client_logs.txt"

typedef struct {
    FILE *file;
    pthread_mutex_t mutex;
} logs_file_t;

int open_logs_file(logs_file_t *server_logs_file, const char *filename);
int close_logs_file(logs_file_t *server_logs_file);
int log_sensor_data(logs_file_t *server_logs_file, proto_sensor_data_t *sensor_data, uint32_t thread_id);

#endif // _COMMON_H_