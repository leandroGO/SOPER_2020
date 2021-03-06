/**
 * Fichero: shm_producer_file.c
 *
 * Autores: Leandro Garcia (leandro.garcia@estudiante.uam.es)
 *          Fabian Gutierrez (fabian.gutierrez@estudiante.uam.es)
 * Grupo: 2201
 * Fecha: 10/04/2020
 * Descripcion: Version de shm_producer con escritura en fichero.
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include "shm_producer_consumer.h"

void clean_up(Info *info, char *name, int ret) {
    if (info != NULL) {
        munmap(info, sizeof(*info));
    }

    if (name != NULL) {
        unlink(name);
    }

    exit(ret);
}

int main(int argc, char *argv[]) {
    int N, rand_flag;
    int fd;
    int i, aux;
    Info *info;

    if (argc != 3 || (N = atoi(argv[1])) < 0 || (rand_flag = atoi(argv[2])) < 0 || rand_flag > 1) {
        printf("ERROR: deberia ser %s <N> <rand_flag> (con N no negativo y rand_flag 0 o 1)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    /*Creando fichero*/
    fd = open(FILE_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, sizeof(Info)) < 0) {
        perror("ftruncate");
        clean_up(NULL, FILE_NAME, EXIT_FAILURE);
    }

    /*Mapeando*/
    info = mmap(NULL, sizeof(Info), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (info == MAP_FAILED) {
        perror("mmap");
        clean_up(NULL, FILE_NAME, EXIT_FAILURE);
    }

    /*Inicializando fichero*/
    if (sem_init(&info->mutex, 1, 1) < 0) {
        perror("sem_init (mutex)");
        clean_up(info, FILE_NAME, EXIT_FAILURE);
    }

    if (sem_init(&info->empty, 1, Q_SIZE) < 0) {
        perror("sem_init (empty)");
        sem_destroy(&info->mutex);
        clean_up(info, FILE_NAME, EXIT_FAILURE);
    }

    if (sem_init(&info->fill, 1, 0) < 0) {
        perror("sem_init (fill)");
        sem_destroy(&info->mutex);
        sem_destroy(&info->empty);
        clean_up(info, FILE_NAME, EXIT_FAILURE);
    }

    info->queue.rear = info->queue.front = 0;

    /*Produciendo*/
    for (i = 0; i < N; i++) {
        switch (rand_flag) {
            case 0: aux = rand() % SUP; break;
            case 1: aux = i % SUP; break;
            default: aux = -1;
        }
        sem_wait(&info->empty);
        sem_wait(&info->mutex);
        info->queue.data[info->queue.rear] = aux;
        info->queue.rear = (info->queue.rear + 1) % Q_SIZE;
        sem_post(&info->mutex);
        sem_post(&info->fill);
    }

    sem_wait(&info->empty);
    sem_wait(&info->mutex);
    info->queue.data[info->queue.rear] = -1;    //ultimo elemento
    info->queue.rear = (info->queue.rear + 1) % Q_SIZE;
    sem_post(&info->mutex);
    sem_post(&info->fill);

    /*Terminando*/
    clean_up(info, NULL, EXIT_SUCCESS);
}