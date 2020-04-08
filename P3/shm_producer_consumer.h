#ifndef SHM_PRODUCER_CONSUMER_H
#define SHM_PRODUCER_CONSUMER_H

#define SHM_NAME "/shm_producer_consumer"
#define Q_SIZE 32
#define SUP 10   //trabajamos con los numeros de 0 a SUP-1

typedef struct _Queue {
    int data[Q_SIZE];
    int front;
    int rear;
} Queue;

typedef struct _Info {
    sem_t *mutex;
    sem_t *empty;
    sem_t *fill;
    Queue queue;
} Info;

#endif