/**
 * Fichero: shm_consumer.c
 *
 * Autores: Leandro Garcia (leandro.garcia@estudiante.uam.es)
 *          Fabian Gutierrez (fabian.gutierrez@estudiante.uam.es)
 * Grupo: 2201
 * Fecha: 10/04/2020
 * Descripcion: Programa que implementa el consumidor del problema
 *  de productor-consumidor.
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
#include "shm_producer_consumer.h"

int main(int argc, char *argv[]) {
    int hist[SUP] = {0};
    int i, aux;
    int fd;
    Info *info;

    /*Accediendo a la memoria compartida*/
    fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (fd < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    /*Mapeando*/
    info = mmap(NULL, sizeof(Info), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (info == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /*Consumiendo*/
    do {
        sem_wait(&info->fill);
        sem_wait(&info->mutex);
        aux = info->queue.data[info->queue.front];
        info->queue.front = (info->queue.front + 1) % Q_SIZE;
        sem_post(&info->mutex);
        sem_post(&info->empty);

        if (aux >= 0) {
            hist[aux]++;
        }
    } while (aux != -1);

    /*Imprimiendo histograma*/
    for (i = 0; i < SUP; i++){
        printf("%d -> %d\n", i, hist[i]);
    }

    /*Terminando*/
    sem_destroy(&info->mutex);
    sem_destroy(&info->empty);
    sem_destroy(&info->fill);
    shm_unlink(SHM_NAME);
    munmap(info, sizeof(*info));
    exit(EXIT_SUCCESS);
}