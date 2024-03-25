#include <stdio.h>
#include <stdlib.h>

#include "server_logs.h"

#include "common.h"

int open_server_logs_file(logs_file_t *server_logs_file, const char *filename){
    printf("Opening server logs file\n");
    server_logs_file->file = fopen(filename, "a");
    if (server_logs_file->file == NULL) {
        fprintf(stderr, "Error opening server logs file \"%s\"\n", filename);
        return STATUS_ERROR;
    }

    if (pthread_mutex_init(&server_logs_file->mutex, NULL) != 0) {
        fprintf(stderr, "Error initializing mutex for server logs file\n");
        fclose(server_logs_file->file);
        return STATUS_ERROR;
    }

    printf("Server logs file opened\n");

    return STATUS_SUCCESS;
}

int close_server_logs_file(logs_file_t *server_logs_file){
    if (fclose(server_logs_file->file) == EOF) {
        fprintf(stderr, "Error closing server logs file\n");
        return STATUS_ERROR;
    }

    if (pthread_mutex_destroy(&server_logs_file->mutex) != 0) {
        fprintf(stderr, "Error destroying mutex for server logs file\n");
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

int log_server_sensor_data(logs_file_t *server_logs_file, proto_sensor_data_t *sensor_data, uint32_t thread_id){

    pthread_mutex_lock(&server_logs_file->mutex);

    //printf("Logging sensor data\n");

    fprintf(server_logs_file->file, "[TODO_TIMESTAMP] Thread %d received data from sensor %d. Value: %f\n", thread_id,sensor_data->hdr.sensor_id, get_float_value(sensor_data));
    fflush(server_logs_file->file);

    //printf("Sensor Data logged\n");

    pthread_mutex_unlock(&server_logs_file->mutex);

    return STATUS_SUCCESS;
}