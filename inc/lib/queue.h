#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <pthread.h>

#include "unordered_circular_doubly_list_with_base.h"

typedef struct queue {
    unordered_circular_doubly_list_with_base_t *elements;
} queue_t;

queue_t *queue_create(bool (*are_equal)(void *variable1, void *variable2));
void queue_insert(void * element, queue_t *queue);
void *queue_remove(queue_t *queue);
void *queue_query_first(queue_t *queue);
void _destroy(queue_t **queue);
#define queue_destroy(queue) _destroy((queue_t **)(&(queue)))
uint32_t queue_get_number_of_elements(queue_t *queue);
void queue_print_int(queue_t *queue);

#endif /* _QUEUE_H_ */