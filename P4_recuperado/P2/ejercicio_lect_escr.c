/**
 * Fichero: ejercicio_lect_escr.c
 *
 * Autores: Leandro Garcia (leandro.garcia@estudiante.uam.es)
 *          Fabian Gutierrez (fabian.gutierrez@estudiante.uam.es)
 * Grupo: 2201
 * Fecha: 24/03/2020
 * Descripcion: Programa que implementa una solucion al problema
 * de lectores/escritores con prioridad a los lectores.
 */
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define WAIT_N(num_wait) {int i_wait; for (i_wait = 0; i_wait < num_wait; i_wait++) wait(NULL);}
#define N_READ 10
#define SECS 1
#define SEM_W "/sem_w"
#define SEM_R "/sem_r"
#define SEM_COUNT "/sem_count"

static int end = 0; //Bandera que indica la atencion a SIGINT

/**
 * Nombre: manejador_SIGINT
 * 
 * Descripcion: La rutina de atencion a SIGINT.
 * Parametro: sig identificador de la sennal.
 */
void manejador_SIGINT() {
    end = 1;
}

/**
 * Nombre: lectura
 * 
 * Descripcion: Simula una lectura.
 */
void lectura() {
    printf("R-INI %d\n", getpid());
    fflush(stdout);
    sleep(1);
    printf("R-FIN %d\n", getpid());
    fflush(stdout);
}

/**
 * Nombre: escritura
 * 
 * Descripcion: Simula una escritura.
 */
void escritura() {
    printf("W-INI %d\n", getpid());
    fflush(stdout);
    sleep(1);
    printf("W-FIN %d\n", getpid());
    fflush(stdout);
}

/**
 * Nombre: cleanUp
 * 
 * Descripcion: Cierra los semaforos y termina
 *  el proceso.
 * Parametros:
 *  -sem1, sem2, sem3 los semaforos a cerrar.
 *  -status el estado de finalizacion del proceso.
 */
void cleanUp(sem_t *sem1, sem_t *sem2, sem_t *sem3, int status) {
    if (sem1 != NULL) {
        sem_close(sem1);
    }
    if (sem2 != NULL) {
        sem_close(sem2);
    }
    if (sem3 != NULL) {
        sem_close(sem3);
    }
    exit(status);
}

int main(int argc, char** argv) {
    int i, count = 0;
    pid_t pid;
    pid_t hijos[N_READ];
    struct sigaction s_int;
    sigset_t block_sigint;
    sem_t *sem_r, *sem_w, *sem_count;


    /*Establecemos manejador de SIGINT*/
    s_int.sa_handler = manejador_SIGINT;
    s_int.sa_flags = 0;

    sigemptyset(&block_sigint);
    sigaddset(&block_sigint, SIGINT);

    if (sigprocmask(SIG_BLOCK, &block_sigint, NULL) < 0) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    if(sigaction(SIGINT, &s_int, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_UNBLOCK, &block_sigint, NULL) < 0) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    /*Inicializacion de semaforos*/
    if ((sem_r = sem_open(SEM_R, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_unlink(SEM_R);

    if ((sem_w = sem_open(SEM_W, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        perror("sem_open");
        cleanUp(sem_r, NULL, NULL, EXIT_FAILURE);
    }
    sem_unlink(SEM_W);

    if ((sem_count = sem_open(SEM_COUNT, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        perror("sem_open");
        cleanUp(sem_r, sem_w, NULL, EXIT_FAILURE);
    }
    sem_unlink(SEM_COUNT);

    /*Creacion de hijos*/
    for (i = 0; i < N_READ; i++) {
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork: %s\n", strerror(pid));
            for (; i > 0; i--) {
                wait(NULL);
            }
            cleanUp(sem_r, sem_w, sem_count, EXIT_FAILURE);
        }

        else if (pid == 0) {
            /*Hijo*/
            while(1) {
                sem_wait(sem_r);
                sem_post(sem_count);
                if (sem_getvalue(sem_count, &count) == -1) {
                    perror("sem_getvalue");
                    exit(EXIT_FAILURE);
                }
                if (count == 1) {   //LightSwitchOn
                    sem_wait(sem_w);
                }
                sem_post(sem_r);

                lectura();

                sem_wait(sem_r);
                sem_wait(sem_count);
                if (sem_getvalue(sem_count, &count) == -1) {
                    perror("sem_getvalue");
                    exit(EXIT_FAILURE);
                }
                if (count == 0) { //LightSwitchOff
                    sem_post(sem_w);
                }
                sem_post(sem_r);

                sleep(SECS);
            }
            exit(EXIT_SUCCESS);
        }

        else {
            hijos[i] = pid;
        }
    }

    while (end == 0) {
        sem_wait(sem_w);
        escritura();
        sem_post(sem_w);

        sleep(SECS);
    }

    for (i = 0; i < N_READ; i++) {
        if (kill(hijos[i], SIGTERM) < 0) {
            perror("kill");
            WAIT_N(i);
            cleanUp(sem_r, sem_w, sem_count, EXIT_FAILURE);
        }
    }

    WAIT_N(N_READ);
    cleanUp(sem_r, sem_w, sem_count, EXIT_SUCCESS);
}