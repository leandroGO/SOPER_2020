/**
 * Fichero: shm_producer_consumer.h
 *
 * Autores: Leandro Garcia (leandro.garcia@estudiante.uam.es)
 *          Fabian Gutierrez (fabian.gutierrez@estudiante.uam.es)
 * Grupo: 2201
 * Fecha: 10/04/2020
 * Descripcion: Fichero con informacion comun a shm_producer y
 *  shm_consumer (y sus versiones *_file).
 */
#ifndef SHM_PRODUCER_CONSUMER_H
#define SHM_PRODUCER_CONSUMER_H

#include <stdlib.h>
#include <semaphore.h>

#define SHM_NAME "/shm_producer_consumer"
#define FILE_NAME "shm_producer_consumer.dat"
#define SEM_P_NAME "/sem_p"
#define SEM_C_NAME "/sem_c"
#define Q_SIZE 30
#define SUP 10   //trabajamos con los numeros de 0 a SUP-1

/* Cola de enteros */
typedef struct _Queue {
    int data[Q_SIZE];
    int front;
    int rear;
} Queue;

/* Informacion compartida */
typedef struct _Info {
    sem_t mutex;
    sem_t empty;
    sem_t fill;
    Queue queue;
} Info;

#endif