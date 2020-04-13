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

#define WAIT_N(num_wait) {int i_wait; for (i_wait = 0; i_wait < num_wait; i_wait++) wait(NULL);}
#define SHM_NAME "/shm_eje3"
#define MAX_MSG 2000

typedef struct {
	pid_t processid;       /* Logger process PID */
	long logid;            /* Id of current log line */
	char logtext[MAX_MSG]; /* Log text */
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
	}
}

int main(int argc, char** argv) {   
    int N, M, i, j, t;
    pid_t pid, padre;
    int shm;
    struct sigaction s_u2;
    char str[MAX_MSG];

    shm_unlink(SHM_NAME);//Quitar

    if (argc != 3 || (N = atoi(argv[1])) < 0 || (M = atoi(argv[2])) < 0)  {
        printf("%s <N> <M>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*Reserva y mapeo de memoria*/
    shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (shm == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm, sizeof(ClientLog)) == -1) {
        perror("ftruncate");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    if ((shm_struct = mmap(NULL, sizeof(ClientLog), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0)) == MAP_FAILED) {
        perror("mmap");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }
    close(shm);

    /*Inicializacion de la estructura*/
    shm_struct->logid = -1;
    shm_struct->logtext[0] = '\0';
    shm_struct->processid = -1;

    /*Establecimiento del manejador*/
    s_u2.sa_handler = manejador;
    s_u2.sa_flags = 0;
    sigemptyset(&(s_u2.sa_mask));

    if (sigaction(SIGUSR2, &s_u2, NULL) < 0) {
        perror("sigaction");
        shm_unlink(SHM_NAME);
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

                shm_struct->processid = getpid();
                shm_struct->logid++;
                getMilClock(str);
                sprintf(str, "Soy el proceso %d a las %s", getpid(), str);
                if (strncpy(shm_struct->logtext, str, MAX_MSG) == NULL) {
                    exit(EXIT_FAILURE);
                }

                if (kill(padre, SIGUSR2) < 0) {
                    perror("kill");
                    exit(EXIT_FAILURE);
                }
            }

            exit(EXIT_SUCCESS);
        }
    }

    /*Rutina padre*/
    printf("1"); fflush(stdout);
    WAIT_N(N);
    munmap(shm_struct, sizeof(ClientLog));
    shm_unlink(SHM_NAME);
    exit(EXIT_SUCCESS);
}