#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

void manejador_SIGALRM(int sig) {}

void manejador_SIGTERM(int sig) {
    printf("Finalizado %d\n", getpid());
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void trabajo() {
    int i;
    int suma;
    pid_t pid = getpid();

    for (i = 1, suma = 0; i < pid/10; i++)
        suma += i;

    printf("PID:%d SUM:%d\n", pid, suma);
    fflush(stdout);
}

int main(int argc, char **argv) {
    int N;
    int T;
    int i;
    int error;
    pid_t ppid = getpid();
    pid_t *hijos;
    struct sigaction s_alarm;
    struct sigaction s_term;
    sigset_t block_alarm, wait_alarm;
    sigset_t block_term, wait_term;
    sigset_t oldset, allset;

    if (argc != 3 || (N = atoi(argv[1])) < 0 || (T = atoi(argv[2])) < 0) {
        fprintf(stdout, "ERROR: Deberia ser %s <N> <T> (con N,T no negativos).\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((hijos = (pid_t*)malloc(N * sizeof(pid_t))) == NULL) { //Esta memoria se libera al hacer exit
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /*Inicializando los struct sigaction y sigset_t*/
    s_alarm.sa_handler = manejador_SIGALRM;
    s_alarm.sa_flags = 0;
    sigemptyset(&(s_alarm.sa_mask));

    s_term.sa_handler = manejador_SIGTERM;
    s_term.sa_flags = 0;
    sigemptyset(&(s_term.sa_mask));

    sigemptyset(&block_alarm);
    sigemptyset(&block_term);
    sigaddset(&block_alarm, SIGALRM);
    sigaddset(&block_term, SIGTERM);

    sigfillset(&wait_alarm);
    sigfillset(&wait_term);
    sigdelset(&wait_alarm, SIGALRM);
    sigdelset(&wait_term, SIGTERM);

    sigfillset(&allset);

    /*Creando hijos*/
    for (i = 0; i < N; i++) {
        if ((error = fork()) < 0) {
            fprintf(stderr, "fork: %s\n", strerror(error));
            for (; i > 0; i--) {
                wait(NULL);
            }
            exit(EXIT_FAILURE);
        }
        if (!error) {
            trabajo();
            /*if (kill(ppid, SIGUSR2) < 0) {
                perror("kill");
                exit(EXIT_FAILURE);
            }*/
            /*Definiendo manejador_SIGTERM como rutina de tratamiento*/
            if (sigprocmask(SIG_BLOCK, &allset, &oldset) < 0) {
                perror("sigprocmask");
                exit(EXIT_FAILURE);
            }
            if (sigaction(SIGTERM, &s_term, NULL) < 0) {
                perror("sigaction");
                exit(EXIT_FAILURE);
            }
            sigsuspend(&wait_term);
        } else {
            hijos[i] = error;
        }
    }

    /*Espera*/
    if (sigprocmask(SIG_BLOCK, &allset, &oldset) < 0) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGALRM, &s_alarm, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (alarm(T)) {
        fprintf(stderr, "Existe una alarma previa establecida\n");
    }
    sigsuspend(&wait_alarm);

    if (sigprocmask(SIG_UNBLOCK, &oldset, NULL) < 0) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    /*Enviando señales*/
    for (i = 0; i < N; i++) {
        if (kill(hijos[i], SIGTERM) < 0) {
            perror("kill");
            for (i = 0; i < N; i++)
                wait(NULL);
            exit(EXIT_FAILURE);
        }
    }

    /*Salida*/
    printf("Finalizado padre\n");
    fflush(stdout);
    for (i = 0; i < N; i++)
        wait(NULL);
    exit(EXIT_SUCCESS);
}