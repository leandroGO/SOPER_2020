/**
 * Fichero: ejercicio_kill.c
 *
 * Autores: Leandro Garcia (leandro.garcia@estudiante.uam.es)
 *          Fabian Gutierrez (fabian.gutierrez@estudiante.uam.es)
 * Grupo: 2201
 * Fecha: 05/03/2020
 * Descripcion: Programa que envia una sennal a un proceso, ambos
 *  indicados en los argumentos.
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char** argv){
    int signal, pid;
    
    if (argc != 3) {
        printf("Entrada esperada: %s <signal> <pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal = atoi(argv[1]);
    pid = atoi(argv[2]);

    if (kill(pid, signal) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}