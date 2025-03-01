/**
 * @file common.c
 * @brief Common functions used by both the server and the client
 * 
 * This file contains common functions, structures and macros used by both the server and the client
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path src/common.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#include "common.h"

int validate_port(int server_port) {
    if (server_port < 1024 || server_port > 65535) {
        fprintf(stderr, "Invalid port number. Should be between 1024 and 65535.\n");
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

void disable_buffering(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

int close_socket(int server_socket){
    if (close(server_socket) == -1) {
        fprintf(stderr, "Error closing socket\n");
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

float get_float_value(proto_sensor_data_t *data){
    return *((float *)&(data->data));
}

int generate_random_float(float *p_float_out) {
    if (p_float_out == NULL) {
        fprintf(stderr, "Error generating random float: pointer can not be null\n");
        return STATUS_ERROR;
    }

    *p_float_out = ((float)rand() / RAND_MAX) * 100.0;

    return STATUS_SUCCESS; 
}

int get_current_time(char **buffer) {
    time_t rawtime;
    struct tm *timeinfo;

    *buffer = (char *)calloc(TIME_BUFFER_SIZE, sizeof(char));
    if (*buffer == NULL) {
        fprintf(stderr, "Error allocating memory for time buffer\n");
        return STATUS_ERROR;
    }

    // Get current time
    time(&rawtime);
    if (rawtime == -1) {
        fprintf(stderr, "Error getting current time\n");
        free(*buffer);
        return STATUS_ERROR;
    }
    timeinfo = localtime(&rawtime);
    if (timeinfo == NULL) {
        fprintf(stderr, "Error getting local time\n");
        free(*buffer);
        return STATUS_ERROR;
    }

    // Format time
    size_t bytes = strftime(*buffer, TIME_BUFFER_SIZE, "%d-%m-%Y %H:%M:%S", timeinfo);
    if (bytes == 0) {
        fprintf(stderr, "Error formatting time\n");
        free(*buffer);
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

int join_threads(pthread_t *tids){
    //printf("Joining threads\n");

    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++){
        if (pthread_join(tids[i], NULL) != 0){
            fprintf(stderr, "Error joining thread %d\n", i);
            return STATUS_ERROR;
        }
    }

    //printf("Threads joined\n");
    return STATUS_SUCCESS;
}

int _open_logs_files(int logs_files_flag, logs_file_t logs_file[], const char *format) {

    if (logs_files_flag == 0) {
        return STATUS_SUCCESS;
    }

    if (logs_file == NULL) {
        fprintf(stderr, "Error opening logs file: logs_file can not be null\n");
        return STATUS_ERROR;
    }

    if (format == NULL) {
        fprintf(stderr, "Error opening logs file: format can not be null\n");
        return STATUS_ERROR;
    }

    printf("Opening logs file...\n");
    
    int size = 0;
    size_t needed_size = 0;

    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++) {
        size = snprintf(NULL, 0, format, i);

        if (size <= 0) {
            fprintf(stderr, "Error calculating size of logs file name of sensor %d\n", i);
            _close_n_logs_files(logs_files_flag, logs_file, i);
            return STATUS_ERROR;
        }

        needed_size = (size_t) (size + 1); // +1 for the null terminator

        logs_file[i].filename = (char *) calloc(needed_size, sizeof(char));
        if (logs_file[i].filename == NULL) {
            fprintf(stderr, "Error allocating memory for logs file name of sensor %d\n", i);
            _close_n_logs_files(logs_files_flag, logs_file, i);
            return STATUS_ERROR;
        }

        snprintf(logs_file[i].filename, needed_size, format, i);

        logs_file[i].file = fopen(logs_file[i].filename, "a");
        if (logs_file[i].file == NULL) {
            fprintf(stderr, "Error opening logs file \"%s\" from sensor %d\n", logs_file[i].filename, i);
            _close_n_logs_files(logs_files_flag, logs_file, i);
            if (logs_file[i].filename != NULL){
                free(logs_file[i].filename);
                logs_file[i].filename = NULL;
            }
            return STATUS_ERROR;
        }
    }

    return STATUS_SUCCESS;
}

int open_server_logs_files(int logs_files_flag, logs_file_t logs_file[]){
    return _open_logs_files(logs_files_flag, logs_file, "logs/srv/sensor_%u_server_logs.txt");
}

int open_client_logs_files(int logs_files_flag, logs_file_t logs_file[]){
    return _open_logs_files(logs_files_flag, logs_file, "logs/cli/sensor_%u_client_logs.txt");
}

int _close_n_logs_files(int logs_files_flag, logs_file_t logs_file[], uint32_t n) {

    if (logs_files_flag == 0) {
        return STATUS_SUCCESS;
    }

    if (logs_file == NULL) {
        fprintf(stderr, "Error closing logs file: logs_file can not be null\n");
        return STATUS_ERROR;
    }

    int error_flag = 0;
    for (uint32_t i = 0; i < n; i++) {
        if (logs_file[i].file != NULL) {
            if (fclose(logs_file[i].file) == EOF) {
                fprintf(stderr, "Error closing logs file \"%s\" from sensor %d\n", logs_file[i].filename, i);
                error_flag = 1;
            }
            logs_file[i].file = NULL;
        }

        if (logs_file[i].filename != NULL) {
            free(logs_file[i].filename);
            logs_file[i].filename = NULL;
        }
    }

    return error_flag ? STATUS_ERROR : STATUS_SUCCESS;
}

int close_logs_files(int logs_files_flag, logs_file_t logs_file[]){
    return _close_n_logs_files(logs_files_flag, logs_file, NUMBER_OF_SENSORS);
}

int log_server_sensor_data(logs_file_t *logs_file, proto_sensor_data_t *sensor_data, uint32_t thread_id){
    return _log_sensor_data(logs_file, sensor_data, thread_id, "[%s] Thread %d received data from sensor %d. Value: %f\n");
}

int log_client_sensor_data(logs_file_t *logs_file, proto_sensor_data_t *sensor_data, uint32_t thread_id){
    return _log_sensor_data(logs_file, sensor_data, thread_id, "[%s] Thread %d sent data from sensor %d. Value: %f\n");
}


int _log_sensor_data(logs_file_t *logs_file, proto_sensor_data_t *sensor_data, uint32_t thread_id, const char* format){
    //printf("Logging sensor data\n");
    
    if (logs_file == NULL) {
        fprintf(stderr, "Error logging sensor data: logs_file can not be null\n");
        return STATUS_ERROR;
    }

    if (sensor_data == NULL) {
        fprintf(stderr, "Error logging sensor data: sensor_data can not be null\n");
        return STATUS_ERROR;
    }

    if (format == NULL) {
        fprintf(stderr, "Error logging sensor data: format can not be null\n");
        return STATUS_ERROR;
    }

    char *time = NULL;
    if (get_current_time(&time) == STATUS_ERROR) {
        fprintf(stderr, "Error getting current time\n");
        return STATUS_ERROR;
    }

    fprintf(logs_file->file, format, time, thread_id,sensor_data->hdr.sensor_id, get_float_value(sensor_data));
    fflush(logs_file->file);

    free(time);
    time = NULL;
    
    //printf("Sensor Data logged\n");

    return STATUS_SUCCESS;
}