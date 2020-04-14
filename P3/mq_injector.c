/**
 * Fichero: mq_injector.c
 *
 * Autores: Leandro Garcia (leandro.garcia@estudiante.uam.es)
 *          Fabian Gutierrez (fabian.gutierrez@estudiante.uam.es)
 * Grupo: 2201
 * Fecha: 14/04/2020
 * Descripcion: Programa que agrega a una cola de mensajes el contenido
 *  de un fichero por bloques de cierta longitud.
 */
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <mqueue.h>

#define MAX_MSG 2048

int main(int argc, char** argv) {
    char buff[MAX_MSG];
    FILE *f;
    mqd_t queue;
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = MAX_MSG * sizeof(char)
    };

    if (argc != 3) {
        printf("%s <fichero_entrada> <nombre_cola>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*Apertura fichero y cola*/
    f = fopen(argv[1], "r");
    if (f == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    queue = mq_open(argv[2], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR, &attributes);
    if (queue == (mqd_t)-1) {
        perror("mq_open");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    while (fread(&buff, sizeof(char), MAX_MSG, f) > 0) {
        if (mq_send(queue, buff, sizeof(buff), 1) == -1) {
            perror("mq_send");
            mq_close(queue);
            fclose(f);
            exit(EXIT_FAILURE);
        }
    }

    buff[0] = EOF;
    if (mq_send(queue, buff, sizeof(buff), 1) == -1) {
        perror("mq_send");
        mq_close(queue);
        fclose(f);
        exit(EXIT_FAILURE);
    }

    fclose(f);
    mq_close(queue);
    exit(EXIT_SUCCESS);
}