/**
 * @file common.h
 * @brief Header file for the common functions, structures and macros
 * 
 * This header file provides the prototypes of the common functions, structures and macros used by the server and the client
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path inc/common.h
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <pthread.h>

#define STATUS_ERROR   ( (int) (-1) )
#define STATUS_SUCCESS ( (int) (0)  )

//#define MAX_PENDING_CONECTIONS (2048)
#define MAX_BUFFER_SIZE (1024)

#define NUMBER_OF_SENSORS ( (uint32_t) (5) )

#define SENSOR_RATE (10000) // Define the desired frequency in Hz
#define DELAY_MS (1000 / SENSOR_RATE) // Calculate delay in milliseconds

#define TIME_BUFFER_SIZE (20) // Size of the buffer to store the time string


/**
 * This type defines the possible types of messages that can be sent between the server and the clients
 * @brief Types of messages that can be sent between the server and the clients
 * @param PROTO_SENSOR_DATA Message that contains the sensor data sent by the client to the server
 * @typedef proto_type_t
 */
typedef uint32_t proto_type_t;

/**
 * This macro defines the type of the message that contains the sensor data sent by the client to the server
 * @brief Type of the message that contains the sensor data sent by the client to the server
 * @note This macro is used to set the type field of the proto_hdr_t structure when sending the sensor data from the client to the server
 */
#define PROTO_SENSOR_DATA ( (proto_type_t) (0u))

/**
 * This structure defines the header of the messages that will be exchanged between the server and the clients
 * @brief Header of the messages
 * @param type Type of the message (proto_type_t)
 * @param sensor_id Sensor ID
 * @param len Length of the data 
 * @struct proto_hdr
 * @typedef proto_hdr_t
 */
typedef struct proto_hdr {
    proto_type_t type;
    uint32_t sensor_id;
    uint16_t len;
} __attribute__((packed)) proto_hdr_t;
_Static_assert(sizeof(proto_hdr_t) == 10, "proto_hdr_t must be 10 bytes");

/**
 * This structure defines the messages that the client will send to the server with the sensor data
 * @brief Messages exchanged between the server and the clients
 * @param hdr Header of the message
 * @param data Data of the message
 * @struct proto_sensor_data
 * @typedef proto_sensor_data_t
 */
typedef struct proto_sensor_data{
    proto_hdr_t hdr;
    uint32_t data;
} __attribute__((packed)) proto_sensor_data_t;
_Static_assert(sizeof(proto_sensor_data_t) == 14, "proto_sensor_data_t must be 14 bytes");

/**
 * This function validates the port number received as an argument via gengetopt
 * @brief Validate the port number
 * @param server_port Port number to be validated received as an argument via gengetopt
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int validate_port(int server_port);

/**
 * This function disables the buffering of the stdout and stderr streams
 * @brief Disable the buffering of the stdout and stderr streams
 */
void disable_buffering(void);

/**
 * This function closes the socket
 * @brief Close the socket
 * @param server_socket Server socket file descriptor
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int close_socket(int server_socket);

/**
 * This function extracts the float value from the proto_sensor_data_t structure
 * @brief Get the float value from the proto_sensor_data_t structure
 * @param data Pointer to the proto_sensor_data_t structure
 * @return Float value
 */
float get_float_value(proto_sensor_data_t *data);

/**
 * This function generates a random float value
 * @brief Generate a random float value
 * @param p_float_out Pointer to the float variable where the random value will be stored
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int generate_random_float(float *p_float_out);

/**
 * This function gets the current time in the format "YYYY-MM-DD HH:MM:SS"
 * @brief Get the current time
 * @param buffer Pointer to the buffer where the time string will be stored
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 * @note The buffer must be freed by the caller
 */
int get_current_time(char **buffer);

/**
 * This function joins the server/client threads 
 * @brief Join the threads
 * @param tids Pointer to the array of pthread_t structures
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int join_threads(pthread_t *tids);

/**
 * This structure defines the file where the logs will be stored
 * @brief File where the logs will be stored. For example: server thread with ID 0 will write the logs to a file named "sensor_0_server_logs.txt" 
 * @param file Pointer to the file (FILE *)
 * @param filename Pointer to the string that contains the file name
 * @struct logs_file_t
 */
typedef struct {
    FILE *file;
    char *filename;
} logs_file_t;

int _open_logs_files(int logs_files_flag, logs_file_t logs_file[], const char *format);

int open_server_logs_files(int logs_files_flag, logs_file_t logs_file[]); 

int open_client_logs_files(int logs_files_flag, logs_file_t logs_file[]);

int _close_n_logs_files(int logs_files_flag, logs_file_t logs_file[], uint32_t n);

int close_logs_files(int logs_files_flag, logs_file_t logs_file[]);

/**
 * This function logs the sensor data received by the server
 * @brief Log the sensor data
 * @param server_logs_file Pointer to the logs_file_t structure what contains the file descriptor of the logs file and a mutex to protect the file descriptor
 * @param sensor_data Pointer to the proto_sensor_data_t structure that contains the sensor data
 * @param thread_id ID of the thread that received the sensor data
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int log_server_sensor_data(logs_file_t *server_logs_file, proto_sensor_data_t *sensor_data, uint32_t thread_id);

/**
 * This function logs the sensor data received by the client
 * @brief Log the sensor data
 * @param logs_file Pointer to the logs_file_t structure what contains the file descriptor of the logs file and a mutex to protect the file descriptor
 * @param sensor_data Pointer to the proto_sensor_data_t structure that contains the sensor data
 * @param thread_id ID of the thread/sensor that sent the sensor data
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int log_client_sensor_data(logs_file_t *logs_file, proto_sensor_data_t *sensor_data, uint32_t thread_id);


#endif // _COMMON_H_

// Path: inc/common.h