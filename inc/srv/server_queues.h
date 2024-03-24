#ifndef _SERVER_QUEUES_H_
#define _SERVER_QUEUES_H_

#include "queue_thread_safe.h"

int create_queues(queue_thread_safe_t **queues);
int destroy_queues(queue_thread_safe_t **queues);
int print_queue(const queue_thread_safe_t *queues);

#endif // _SERVER_QUEUES_H_