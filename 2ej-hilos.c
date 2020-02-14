#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define SUP 10
#define INF 0

typedef struct{
    pthread_t thread;
    int slp;
    int x;
} Hilo;

void clean_up(Hilo *hilos, int **ret, int num) {
    int i;
    
    if (hilos) {
        for (i = 0; i < num; i++) {
            pthread_cancel(hilos[i].thread);
        }

        free(hilos);
    } 

    if (ret != NULL) {
        for (i = 0; i < num; i++) {
            if (ret[i]) free(ret[i]);
        }
        free(ret);
    }
}

void fun(Hilo h) {
    int *ret;

    ret = (int*)malloc(sizeof(int));
    if (ret == NULL) {
        perror("Malloc: ");
        pthread_exit(NULL);
    }

    sleep(h.slp);
    pthread_exit(x^3);
}

int main(int argc, char** argv) {
    int num, i;
    Hilo *hilos;
    int **ret_val;

    if (argc != 2 || (num = atoi(argv[1])) < 1) {
        printf("Parametros erroneos");
        return EXIT_FAILURE;
    }

    hilos = (Hilo*)malloc(num*sizeof(Hilo));
    if (hilos == NULL) {
        perror("Malloc: ");
        return EXIT_FAILURE;
    }

    ret_val = (int**)malloc(num*sizeof(int*));
    if (ret_val == NULL) {
        perror("Malloc: ");
        free(hilos);
        return EXIT_FAILURE;
    }

    for (i = 0; i < num; i++) {
        hilos[i].x = i;
        hilos[i].slp = (rand()/(RAND_MAX + 1.))*(SUP - INF + 1) + INF;

        if (pthread_create(hilos[i].thread, NULL, fun, hilos[i]) != 0) {
            perror("Pthread_create: ");
            clean_up(hilos, ret_val, i-1);
            return EXIT_FAILURE;
        }
    }

    for (i = 0; i < num; i++) {
        if (pthread_join(hilos[i].thread, (void**)ret_val[i]) != 0) {
            perror("Pthread_join: ");
            clean_up(hilos, ret_val, num);
            return EXIT_FAILURE;
        }
        printf("%d\n", *(ret_val[i]));
    }

    return EXIT_SUCCESS;
}