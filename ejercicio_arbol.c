/**
 * Fichero: ejercicio_arbol.c
 *
 * Autores: Leandro Garcia (leandro.garcia@estudiante.uam.es)
 *          Fabian Gutierrez (fabian.gutierrez@estudiante.uam.es)
 * Grupo: 2201
 * Fecha: 11/02/2020
 * Descripcion: Programa que genera NUM_PROC hijos en cascada
 *  (como se indica para el ejercicio 8).
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_PROC 5

int main(int argc, char** argv) {
    int i;
    pid_t f;

    for (i = 0; i <= NUM_PROC; i++) {
        f = fork();
        if (f < 0) {
            exit(EXIT_FAILURE);
        }
        if (f > 0) {
            break;
        }
        
        #ifdef SLOW
        sleep(1);
        #endif
    }

    wait(NULL);
    exit(EXIT_SUCCESS);
}