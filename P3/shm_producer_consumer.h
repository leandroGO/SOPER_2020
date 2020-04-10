#ifndef SHM_PRODUCER_CONSUMER_H
#define SHM_PRODUCER_CONSUMER_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>

#define SHM_NAME "/shm_producer_consumer"
#define SEM_P_NAME "/sem_p"
#define SEM_C_NAME "/sem_c"
#define Q_SIZE 30
#define SUP 10   //trabajamos con los numeros de 0 a SUP-1

typedef struct _Queue {
    int data[Q_SIZE];
    int front;
    int rear;
} Queue;

typedef struct _Info {
    sem_t mutex;
    sem_t empty;
    sem_t fill;
    Queue queue;
} Info;
/*
sem_t* open_semaphore(char *name) {
    sem_t *sem;

    if ((sem = sem_open(name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        if (errno == EEXIST) {
            if ((sem = sem_open(name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
                exit(EXIT_FAILURE);
            }
            sem_unlink(name);   //Si un proceso lo crea, el siguiente le hace unlink
        } else {
            exit(EXIT_FAILURE);
        }
    }

    return sem;
}*/

#endif