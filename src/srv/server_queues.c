/**
 * @file server_queues.c
 * @brief Implementation of the server queues
 * 
 * This file implements the functions to create and destroy the server queues
 * 
 * @date 27/03/2024
 * @authors Francisco Pedrosa
 * @path src/srv/server_queues.c
 */
#include <stdio.h>
#include <stdlib.h>

#include "server_queues.h"

#include "common.h"

#include "doubly_node.h"
#include "unordered_circular_doubly_list_with_base.h"
#include "queue.h"
#include "queue_thread_safe.h"

int create_queues(queue_thread_safe_t **queues){
    printf("Creating queues...\n");

    for (uint32_t i = 0; i < NUMBER_OF_SENSORS; i++){
        queues[i] = queue_create_thread_safe();
        if (queues[i] == NULL){
            fprintf(stderr, "Error creating queue %d\n", i);
            _destroy_n_queues(queues, i);
            return STATUS_ERROR;
        }
    }

    printf("Queues created successfully.\n");

    return STATUS_SUCCESS;
}

int _destroy_n_queues(queue_thread_safe_t **queues, uint32_t n){
    printf("Destroying queues\n");

    for (uint32_t i = 0; i < n; i++){
        //printf("Destroying queue %d\n", i);
        queue_destroy_thread_safe(queues[i]);
    }

    printf("%d queues destroyed\n", n);

    return STATUS_SUCCESS;
}

int destroy_queues(queue_thread_safe_t **queues){
    return _destroy_n_queues(queues, NUMBER_OF_SENSORS);
}

int print_queue(const queue_thread_safe_t *queue){

    unordered_circular_doubly_list_with_base_t *list = queue->elements;

    if (list == NULL) {
        fprintf(stderr, "[%d@%s][ERROR] The list must not be NULL\n", __LINE__, __FILE__);
        return STATUS_ERROR;
    }

    doubly_node_t *base_node = list->base_node;
    if (base_node == NULL) {
        fprintf(stderr, "[%d@%s][ERROR] Base node is NULL\n", __LINE__, __FILE__);
        return STATUS_ERROR;
    }

    if (list->number_of_elements == 0 && (base_node->next != base_node && base_node->previous != base_node)) {
        fprintf(stderr, "[%d@%s][ERROR] The list has been corrupted\n", __LINE__, __FILE__);
        return STATUS_ERROR;
    }

    
    printf("-----------------------------------------------------\n");
	printf("List addr   : %p\n", (void *) list );
    printf("Number of elements: %d\n", list->number_of_elements);
	printf("Base node: %p (Prev: %p  | Node addr: %p | Next: %p)\n", (void *) base_node->element, (void *) base_node->previous, (void *) base_node, (void *) base_node->next);


    if (list->number_of_elements < 1) {
        return STATUS_SUCCESS;
    }
    
    unsigned int aux = 0;
    doubly_node_t *current = base_node->next;
    while (current != base_node) {

        proto_sensor_data_t *data = (proto_sensor_data_t *) current->element;

        printf("List[%d]: Sensor id: %d, Value: %f, (Prev: %p  | Node addr: %p | Next: %p)\n", aux, data->hdr.sensor_id, get_float_value(data), (void *) current->previous, (void *) current, (void *) current->next);
        current = current->next;
        aux++;
    }
    
	printf("-----------------------------------------------------\n");

    return STATUS_SUCCESS;
    
}

// Path: src/srv/server_queues.c