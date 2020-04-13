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
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define SHM_NAME "/shm_eje3"
#define MAX_MSG 2000

typedef struct {
	pid_t processid;       /* Logger process PID */
	long logid;            /* Id of current log line */
	char logtext[MAX_MSG]; /* Log text */
    sem_t mutex;
} ClientLog;

static ClientLog *shm_struct;

static void getMilClock(char *buf) {
    int millisec;
	char aux[100];
    struct tm* tm_info;
    struct timeval tv;

    gettimeofday(&tv, NULL);
	millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
    if (millisec>=1000) { // Allow for rounding up to nearest second
        millisec -=1000;
        tv.tv_sec++;
    }
    tm_info = localtime(&tv.tv_sec);
    strftime(aux, 10, "%H:%M:%S", tm_info);
	sprintf(buf, "%s.%03d", aux, millisec);
}

void manejador(int sig) {
    if (sig == SIGUSR1) {
		printf ("Log %ld: Pid %d: %s\n", shm_struct->logid, shm_struct->processid, shm_struct->logtext);
        sem_post(&(shm_struct->mutex));
	}
}

int main(int argc, char** argv) {   
    int N, M, i, j, t;
    pid_t pid, padre, *hijos;
    int shm;
    struct sigaction s_u1;
    sigset_t wait_su1, block_su1;
    char str[MAX_MSG], txt[MAX_MSG];

    if (argc != 3 || (N = atoi(argv[1])) < 0 || (M = atoi(argv[2])) < 0)  {
        printf("%s <N> <M>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    hijos = (pid_t*)malloc(N*sizeof(pid_t));
    if (hijos == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /*Reserva y mapeo de memoria*/
    shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (shm == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    shm_unlink(SHM_NAME);

    if (ftruncate(shm, sizeof(ClientLog)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    if ((shm_struct = mmap(NULL, sizeof(ClientLog), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0)) == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    close(shm);

    /*Inicializacion de la estructura*/
    shm_struct->logid = -1;
    shm_struct->logtext[0] = '\0';
    shm_struct->processid = -1;
    if (sem_init(&(shm_struct->mutex), 1, 1) < 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    /*Establecimiento del manejador y mascara*/
    sigfillset(&wait_su1);
    sigdelset(&wait_su1, SIGUSR1);
    sigemptyset(&block_su1);
    sigaddset(&block_su1, SIGUSR1);

    s_u1.sa_handler = manejador;
    s_u1.sa_flags = 0;
    sigemptyset(&(s_u1.sa_mask));

    if (sigaction(SIGUSR1, &s_u1, NULL) < 0) {
        perror("sigaction");
        sem_destroy(&(shm_struct->mutex));
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_BLOCK, &block_su1, NULL) < 0) {
        perror("sigprocmask");
        sem_destroy(&(shm_struct->mutex));
        exit(EXIT_FAILURE);
    }

    /*Creacon de hijos*/
    padre = getpid();
    for (i = 0; i < N; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            /*Rutina hijo*/
            for (j = 0; j < M; j++) {    
                srand(time(NULL));
                t = ((rand()/RAND_MAX)*800000) + 100000;
                usleep(t);

                sem_wait(&(shm_struct->mutex));
                shm_struct->processid = getpid();
                shm_struct->logid++;
                getMilClock(str);
                sprintf(txt, "Soy el proceso %d a las %s", getpid(), str);
                if (strncpy(shm_struct->logtext, txt, MAX_MSG) == NULL) {
                    exit(EXIT_FAILURE);
                }
                
                if (kill(padre, SIGUSR1) < 0) {
                    perror("kill");
                    exit(EXIT_FAILURE);
                }
            }

            exit(EXIT_SUCCESS);
        }

        else {
            hijos[i] = pid;
        }
    }

    /*Rutina padre*/
    for (i = 0; i < M*N; i++) {
        sigsuspend(&wait_su1);
    }

    for (i = 0; i < N; i++) {
        waitpid(hijos[i], NULL, 0);
    }
    munmap(shm_struct, sizeof(ClientLog));
    sem_destroy(&(shm_struct->mutex));
    exit(EXIT_SUCCESS);
}