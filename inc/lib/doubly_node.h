#ifndef __DOUBLY_NODE_H_
#define __DOUBLY_NODE_H_

typedef struct doubly_node {
    struct doubly_node *previous;
    struct doubly_node *next;
    void *element;
} doubly_node_t;

#endif				/* __DOUBLY_NODE_H_ */