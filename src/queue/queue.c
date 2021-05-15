#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

queue_t *init_queue(void) {
    queue_t *queue = malloc(sizeof *queue);
    if (queue == NULL)
        return NULL;

    queue->front = NULL;
    queue->back = NULL;

    return queue;
}

qnode_t *new_node(void *value) {
    qnode_t *node = malloc(sizeof *node);
    if (node == NULL)
        return NULL;

    node->next = NULL;
    node->value = value;

    return node;
}

void insert_string(queue_t *queue, void *value) {
    qnode_t *node = new_node(value);
    if (node == NULL) return;

    if (queue->front == NULL)
        queue->front = node;

    if (queue->back != NULL)
        queue->back->next = node;

    queue->back = node;
}

void *remove_string(queue_t *queue) {
    if (queue->front == NULL) return NULL;

    qnode_t *temp = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL)
        queue->back = NULL;

    void *value = temp->value;

    free(temp);

    return value;
}