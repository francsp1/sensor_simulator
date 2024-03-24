#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

#define STATUS_ERROR   ( (int) (-1) )
#define STATUS_SUCCESS ( (int) (0)  )

#define MAX_PENDING_CONECTIONS (2048)
#define MAX_BUFFER_SIZE (1024)

#define NUMBER_OF_SENSORS ( (uint32_t) (2) )

#define SENSOR_RATE 10 // Define the desired frequency in Hz
#define DELAY_MS (1000 / SENSOR_RATE) // Calculate delay in milliseconds

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

void common(void);

#endif // _COMMON_H_