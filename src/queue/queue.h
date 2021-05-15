// Queue de char array-uri (pentru retinerea coloanelor la SELECT)
//

#ifndef USOSQL_QUEUE_H
#define USOSQL_QUEUE_H

typedef struct NODE {
    struct NODE *next;
    void *value;
} qnode_t;

typedef struct {
    qnode_t *front;
    qnode_t *back;
} queue_t;

queue_t *init_queue(void);

void insert_string(queue_t *queue, void *value);

// se da free la char array in functia care apeleaza remove_string
void *remove_string(queue_t *queue);

// void free_queue(queue_t *queue) ?

#endif //USOSQL_QUEUE_H
