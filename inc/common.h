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

#define SENSOR_RATE (10) // Define the desired frequency in Hz
#define DELAY_MS (1000 / SENSOR_RATE) // Calculate delay in milliseconds

#define TIME_BUFFER_SIZE (20) // Size of the buffer to store the time string

/**
 * This enumeration defines the possible types of messages that can be sent between the server and the clients
 * @brief Enumeration of the possible message types
 * @param PROTO_SENSOR_DATA Exchange sensor data between the server and the clients
 * @enum proto_type_e
 */
typedef enum {
	PROTO_SENSOR_DATA,
} proto_type_e;

/**
 * This structure defines the header of the messages that will be exchanged between the server and the clients
 * @brief Header of the messages
 * @param type Type of the message (proto_type_e)
 * @param sensor_id Sensor ID
 * @param len Length of the data 
 * @struct proto_hdr_t
 */
typedef struct {
	proto_type_e type;
    uint32_t sensor_id;
    uint16_t len;
} proto_hdr_t;

/**
 * This structure defines the messages that the client will send to the server with the sensor data
 * @brief Messages exchanged between the server and the clients
 * @param hdr Header of the message
 * @param data Data of the message
 * @struct proto_sensor_data_t
 */
typedef struct {
    proto_hdr_t hdr;
    uint32_t data;
} proto_sensor_data_t;

/**
 * This function validates the port number received as an argument via gengetopt
 * @brief Validate the port number
 * @param server_port Port number to be validated received as an argument via gengetopt
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int validate_port(int server_port);

/**
 * This function disables the buffering of the stdout and stderr streams
 * @brief Enable buffering
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


#define SERVER_LOGS_FILE "server_logs.txt" //name of the file where the server logs will be stored
#define CLIENT_LOGS_FILE "client_logs.txt" //name of the file where the client logs will be stored

/**
 * This structure defines the file where the logs will be stored
 * @brief File where the logs will be stored
 * @param file Pointer to the file (FILE *)
 * @param mutex Mutex to protect the file
 * @struct logs_file_t
 */
typedef struct {
    FILE *file;
    pthread_mutex_t mutex;
} logs_file_t;

/**
 * This function opens the logs file
 * @brief Open the logs file
 * @param server_logs_file Pointer to the logs_file_t structure what contains the file descriptor of the logs file and a mutex to protect the file descriptor
 * @param filename Name of the file where the logs will be stored
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int open_logs_file(logs_file_t *server_logs_file, const char *filename);

/**
 * This function closes the logs file
 * @brief Close the logs file
 * @param server_logs_file Pointer to the logs_file_t structure what contains the file descriptor of the logs file and a mutex to protect the file descriptor
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 */
int close_logs_file(logs_file_t *server_logs_file);

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

/**
 * This function logs the sensor data
 * @brief Log the sensor data
 * @param logs_file Pointer to the logs_file_t structure what contains the file descriptor of the logs file and a mutex to protect the file descriptor
 * @param sensor_data Pointer to the proto_sensor_data_t structure that contains the sensor data
 * @param thread_id ID of the thread/sensor that sent/received the sensor data
 * @param format Format of the log message
 * @return STATUS_SUCCESS (0) on success, STATUS_FAILURE (-1) on failure
 * @note this function is used by log_server_sensor_data and log_client_sensor_data and should not be called directly
 */
int _log_sensor_data(logs_file_t *logs_file, proto_sensor_data_t *sensor_data, uint32_t thread_id, const char* format);


#endif // _COMMON_H_

// Path: inc/common.h