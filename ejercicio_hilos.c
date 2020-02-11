#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define SUP 10
#define INF 0

typedef struct {
    pthread_t thread;
    int sleep_time;
    int x;
} Hilo;

void clean_up(Hilo *h, int *arr, int ini, int end) {
    int j = 0;
    int error = 0;
    if (h != NULL) {
        for (j = ini; j < end; j++) {
            error = pthread_cancel(h[j], NULL);
            if (error) {
                fprintf(stderr, "pthread_cancel(%d): %s\n", j, strerror(error));
            }
        }
        free(h);
    }
    if (arr != NULL) free(arr);
}

int* funcion(Hilo h) {
    int *ret_val = NULL;

    if ((ret_val = (int*)malloc(sizeof(int))) == NULL) {
        perror("malloc");
        return NULL;
    }


    return ret_val;
}

int main(int argc, char *argv[]) {
    int i = 0;
    int error = 0;
    int **ret_val = NULL;
    Hilo *hilos = NULL;

    if (argc != 2 || argv[1] < 1) {
        printf("ERROR: Deberia ser %s <num_hilos> (con num_hilos > 0).\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*Reserva de memoria*/
    if ((hilos = (Hilo*) malloc(argv[1]*sizeof(Hilo))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if ((ret_val = (int**) malloc(argv[1]*sizeof(int*))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /*Creacion de hilos*/
    for (i = 0; i < argv[1]; i++) {
        hilos[i].x = i + 1;
        hilos[i].sleep_time = (rand()/(RAND_MAX + 1.))*(SUP - INF + 1) + INF;

        error = pthread_create(&hilos[i].thread, NULL, funcion, hilos[i]);
        if (error) {
            fprintf(stderr, "pthread_create(%d): %s\n", i, strerror(error));
            clean_up(hilos, ret_val, 0, i - 1);
            exit(EXIT_FAILURE);
        }
    }

    /*Espera de hilos*/
    for (i = 0; i < argv[1]; i++) {
        error = pthread_join(hilos[i].thread, &ret_val[i]);
        if (error != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(error));
            clean_up(hilos, ret_val, i);
            exit(EXIT_FAILURE);
        }
    }


    printf("El programa %s termino correctamente \n", argv[0]);
    exit(EXIT_SUCCESS);
}