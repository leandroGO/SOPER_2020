#ifndef SHM_PRODUCER_CONSUMER_H
#define SHM_PRODUCER_CONSUMER_H

#define SHM_NAME "/shm_producer_consumer"

typedef struct _Info{
    sem_t *mutex;
    sem_t *empty;
    sem_t *fill;
    
} Info;

#endif