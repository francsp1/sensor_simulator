#ifndef _UNORDERED_CIRCULAR_DOUBLY_LIST_WITH_BASE_H_
#define _UNORDERED_CIRCULAR_DOUBLY_LIST_WITH_BASE_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "doubly_node.h"

typedef struct ucdlwb unordered_circular_doubly_list_with_base_t;

unordered_circular_doubly_list_with_base_t *ucdlwb_create(bool (*are_equal)(void *variable1, void *variable2));

void ucdlwb_free_list_and_all_nodes(unordered_circular_doubly_list_with_base_t *list);

void ucdlwb_print_list_int(const unordered_circular_doubly_list_with_base_t * list);

void ucdlwb_insert_at_start(void *element, unordered_circular_doubly_list_with_base_t *list);
void ucdlwb_insert_at_end(void *element, unordered_circular_doubly_list_with_base_t *list);
void ucdlwb_insert(uint32_t index, void *element, unordered_circular_doubly_list_with_base_t *list);

void *ucdlwb_remove_from_start(unordered_circular_doubly_list_with_base_t *list);
void *ucdlwb_remove_from_end(unordered_circular_doubly_list_with_base_t *list);
void *ucdlwb_remove_by_element(void * element, unordered_circular_doubly_list_with_base_t *list);
void *ucdlwb_remove_by_reference(void * element, unordered_circular_doubly_list_with_base_t *list);
void *ucdlwb_remove_by_index(uint32_t index, unordered_circular_doubly_list_with_base_t *list);

void *ucdlwb_query(uint32_t index, unordered_circular_doubly_list_with_base_t *list);

bool ucdlwb_contains(void * element, unordered_circular_doubly_list_with_base_t *list);

uint32_t ucdlwb_get_number_of_elements(unordered_circular_doubly_list_with_base_t *list);

void ucdlwb_tests();

#endif				/* _UNORDERED_CIRCULAR_DOUBLY_LIST_WITH_BASE_H_ */