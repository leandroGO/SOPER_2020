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
#define WAIT_N(num_wait) {int i_wait; for (i_wait = 0; i_wait < num_wait; i_wait++) wait(NULL);}

void manejador_su2(int sig) {}

void manejador_term(int sig) {}

int main(int argc, char** argv) {
    int N, i, j, count, msgs;
    char clave, buff[MAX_MSG];
    mqd_t queue;
    pid_t pid, padre, *hijos;
    struct sigaction s_u2, s_term;
    sigset_t wait_su2, block_su2, wait_term;
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = sizeof(buff)
    };

    if (argc != 4 || (N = atoi(argv[1])) <= 0 || N > 10) {
        printf("%s <N> <nombre_cola> <caracter>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    clave = argv[3][0];

    hijos = (pid_t*)malloc(N*sizeof(pid_t));
    if (hijos == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /*Apertura de cola de mensajes*/
    queue = mq_open(argv[2], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes);
    if (queue == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    /*Tratamiento de señales*/
    sigfillset(&wait_su2);
    sigfillset(&wait_term);
    sigdelset(&wait_su2, SIGUSR2);
    sigdelset(&wait_term, SIGTERM);

    sigemptyset(&block_su2);
    sigaddset(&block_su2, SIGUSR2);
    sigaddset(&block_su2, SIGTERM);

    s_u2.sa_handler = manejador_su2;
    s_u2.sa_flags = 0;
    sigemptyset(&(s_u2.sa_mask));

    s_term.sa_handler = manejador_term;
    s_term.sa_flags = 0;
    sigemptyset(&(s_term.sa_mask));

    if (sigaction(SIGUSR2, &s_u2, NULL) < 0) {
        perror("sigaction");
        mq_unlink(argv[2]);
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGTERM, &s_term, NULL) < 0) {
        perror("sigaction");
        mq_unlink(argv[2]);
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_BLOCK, &block_su2, NULL) < 0) {
        perror("sigprocmask");
        mq_unlink(argv[2]);
        exit(EXIT_FAILURE);
    } 

    /*Creacion de hijos*/
    padre = getpid();
    for (i = 0; i < N; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            mq_unlink(argv[2]);
            exit(EXIT_FAILURE);
        }

        else if (pid == 0) {
            /*Rutina trabajador*/
            count = 0;
            msgs = 0;
            do {
                if (mq_receive(queue, buff, sizeof(buff), NULL) == -1) {
                    perror("mq_receive");
                    exit(EXIT_FAILURE);
                }
               
                if (buff[0] != EOF) {
                    msgs++;

                    for (j = 0; j < strlen(buff); j++) {
                        if (buff[j] == clave) {
                            count++;
                        }
                    }
                }
            } while (buff[0] != EOF && buff[0] != '~');

            if (buff[0] == EOF) {
                if (kill(padre, SIGUSR2) == -1) {
                    perror("kill");
                    exit(EXIT_FAILURE);
                }

                buff[0] = '~';
                for (i = 1; i < N; i++) {
                    if (mq_send(queue, buff, sizeof(buff), 1) == -1) {
                        perror("mq_send");
                        exit(EXIT_FAILURE);
                    }
                }
            } 

            sigsuspend(&wait_term);
            printf("Mensajes: %d; Coincidencias: %d\n", msgs, count);
            exit(EXIT_SUCCESS);
        }

        hijos[i] = pid;
    }

    /*Finalizacion*/
    sigsuspend(&wait_su2);
    for (i = 0; i < N; i++) {
        if (kill(hijos[i], SIGTERM) == -1) {
            perror("kill");
            mq_unlink(argv[2]);
            exit(EXIT_FAILURE);
        }
    }
    WAIT_N(N);
    mq_unlink(argv[2]);
    mq_close(queue);
    exit(EXIT_SUCCESS);
}