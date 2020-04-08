#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "shm_producer_consumer.h"

int main(int argc, char *argv[]) {
    int hist[SUP] = {0};
    int aux;
    int fd;
    Info *info;

    /*Accediendo a la memoria compartida*/
    fd = shm_open(SHM_NAME, O_RDWR);
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
        sem_wait(info->fill)
        sem_wait(info->mutex)
        aux = info->queue.data[info->queue.front];
        info->queue.front = (info->queue.front + 1) % SUP;
        sem_signal(info->mutex)
        sem_signal(info->empty)

        if (aux >= 0) {
            hist[aux]++;
        }
    } while (aux != -1);

    /*Imprimiendo histograma*/
    for (i = 0; i < SUP; i++){
        printf("%d -> %d\n", i, hist[i]);
    }

    /*Terminando*/
    munmap(info, sizeof(*info));
    exit(EXIT_SUCCESS);
}