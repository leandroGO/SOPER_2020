#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

#define WAIT_N(num_wait) {int i_wait; for (i_wait = 0; i_wait < num_wait; i_wait++) wait(NULL);}
#define DATA "data.txt"
#define SEM_NAME "/sem_rw"
#define SEM_COUNT_NAME "/sem_count"


static sem_t *sem_count = NULL;

void manejador_SIGUSR2(int sig) {}

void manejador_SIGTERM(int sig) {
    printf("Finalizado %d\n", getpid());
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void trabajo(FILE *f, sem_t *sem, int N) {
    int i, n = 0, t = 0;
    int suma;
    pid_t pid = getpid();

    for (i = 1, suma = 0; i < pid/10; i++)
        suma += i;

    printf("PID:%d SUM:%d\n", pid, suma);
    fflush(stdout);

    sem_wait(sem);
    
    if ((f = freopen(DATA, "r", f)) == NULL) {
        exit(EXIT_FAILURE);
    }
    if (fscanf(f, "%d\n%d", &n, &t) < 0) {
        fclose(f);
        exit(EXIT_FAILURE);
    }
    if ((f = freopen(DATA, "w", f)) == NULL) {
        exit(EXIT_FAILURE);
    }
    if (fprintf(f, "%d\n%d", n+1, suma+t) < 0) {
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fflush(f);

    if (n + 1 ==  N) {
        sem_post(sem_count);
    }

    sem_post(sem);
}

int main(int argc, char **argv) {
    FILE *f = NULL;
    int N, i, error, res, foo;
    pid_t *hijos;
    sem_t *sem;
    struct sigaction s_term;
    sigset_t block_term, wait_term, oldset;

    if (argc != 2 || (N = atoi(argv[1])) < 0) {
        fprintf(stdout, "ERROR: Deberia ser %s <N> (con N no negativo).\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    if ((hijos = (pid_t*)malloc(N * sizeof(pid_t))) == NULL) { //Esta memoria se libera al hacer exit
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /*Inicializando los struct sigaction y sigset_t*/
    s_term.sa_handler = manejador_SIGTERM;
    s_term.sa_flags = 0;
    sigemptyset(&(s_term.sa_mask));

    sigemptyset(&block_term);
    sigaddset(&block_term, SIGTERM);

    sigfillset(&wait_term);
    sigdelset(&wait_term, SIGTERM);

    /*Abriendo e inicializando el fichero de datos*/
    f = fopen(DATA, "w");
    if (f == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    if (fprintf(f, "0\n0") < 0) {
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fflush(f);
    

    /*Inicializando semaforos*/
    if ((sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_unlink(SEM_NAME);
    if ((sem_count = sem_open(SEM_COUNT_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    sem_unlink(SEM_COUNT_NAME);

    /*Creando hijos*/
    for (i = 0; i < N; i++) {
        if ((error = fork()) < 0) {
            fprintf(stderr, "fork: %s\n", strerror(error));
            for (; i > 0; i--) {
                wait(NULL);
            }
            fclose(f);
            exit(EXIT_FAILURE);
        }
        if (!error) {
            trabajo(f, sem, N);
            /*Definiendo manejador_SIGTERM como rutina de tratamiento*/
            if (sigprocmask(SIG_BLOCK, &block_term, &oldset) < 0) {
                perror("sigprocmask");
                fclose(f);
                exit(EXIT_FAILURE);
            }
            if (sigaction(SIGTERM, &s_term, NULL) < 0) {
                perror("sigaction");
                fclose(f);
                exit(EXIT_FAILURE);
            }
            sigsuspend(&wait_term);
        } else {
            hijos[i] = error;
        }
    }
   
    sem_wait(sem_count);

    /*Enviando señales*/
    for (i = 0; i < N; i++) {
        if (kill(hijos[i], SIGTERM) < 0) {
            perror("kill");
            WAIT_N(i);
            fclose(f);
            sem_close(sem);
            sem_close(sem_count);
            exit(EXIT_FAILURE);
        }
    }

    if ((f = freopen(DATA, "r", f)) == NULL) {
        WAIT_N(N);
        sem_close(sem);
        sem_close(sem_count);
        exit(EXIT_FAILURE);
    }
    if (fscanf(f, "%d\n%d", &foo, &res) < 0) {
        fclose(f);
        WAIT_N(N);
        sem_close(sem);
        sem_close(sem_count);
        exit(EXIT_FAILURE);
    }

    printf("Han acabado todos, resultado: %d\n", res);

    /*Salida*/
    WAIT_N(N);
    printf("Finalizado padre");
    fflush(stdout);
    fclose(f);
    sem_close(sem);
    sem_close(sem_count);
    exit(EXIT_SUCCESS);
}