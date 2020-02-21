/**
 * Fichero: ejercicio_hilos.c
 *
 * Autores: Leandro Garcia (leandro.garcia@estudiante.uam.es)
 *          Fabian Gutierrez (fabian.gutierrez@estudiante.uam.es)
 * Grupo: 2201
 * Fecha: 15/02/2020
 * Descripcion: Programa que genera hilos como se indica en el
 *  ejercicio 6.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define SUP 10
#define INF 0

/**
 * Almacena la informacion asociada a un hilo que se manipula en
 * funcion().
 */
typedef struct {
    int sleep_time; //Tiempo que debe esperar el hilo
    int x;          //Numero de hilo
} thread_info;

/**
 * Nombre: funcion
 * 
 * Descripcion: Espera un numero de segundos y almacena en un nuevo
 *  entero el cubo de un valor pasado por parametro.
 * Parametro: ti puntero a void (se hace cast a thread_info*) que 
 *  almacena el tiempo a esperar y el valor a elevar al cubo.
 * Retorno: la direccion del entero creado (almacena x^3).
 */
void* funcion(void* ti) {
    int *ret_val = NULL;
    thread_info *cast = (thread_info*)ti;

    sleep(cast->sleep_time);

    if ((ret_val = (int*)malloc(sizeof(int))) == NULL) {
        perror("malloc");
        return NULL;
    }

    *ret_val = cast->x * cast->x * cast->x;

    return (void *)ret_val;
}

int main(int argc, char *argv[]) {
    int n_hilos = 0, i = 0, error = 0;
    int *ret_val = NULL;
    thread_info *info = NULL;
    pthread_t *hilos = NULL;

    if (argc != 2 || (n_hilos = atoi(argv[1])) < 1) {
        fprintf(stdout, "ERROR: Deberia ser %s <num_hilos> (con num_hilos > 0).\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*Reserva de memoria*/
    if ((info = (thread_info*)malloc(n_hilos*sizeof(thread_info))) == NULL) {
        perror("ERROR: malloc");
        free(info);
        exit(EXIT_FAILURE);
    }

    if ((hilos = (pthread_t*)malloc(n_hilos*sizeof(pthread_t))) == NULL) {
        perror("ERROR: malloc");
        free(info);
        exit(EXIT_FAILURE);
    }

    /*Lanzamiento de hilos*/
    for (i = 0; i < n_hilos; i++) {
        info[i].sleep_time = rand() % (SUP - INF + 1) + INF;
        info[i].x = i;

        error = pthread_create(&hilos[i], NULL, funcion, (void *)&info[i]);
        if (error != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(error));
            free(info);
            exit(EXIT_FAILURE);
        }
    }

    /*Recuperacion de retornos*/
    for (i = 0; i < n_hilos; i++) {
        error = pthread_join(hilos[i], (void *)&ret_val);
        if (error != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(error));
            free(info);
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "%d\n", *ret_val);
        free(ret_val);
    }

    free(info);
    printf("El programa %s termino correctamente \n", argv[0]);
    exit(EXIT_SUCCESS);
}
