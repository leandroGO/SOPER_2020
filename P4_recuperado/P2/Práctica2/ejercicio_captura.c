#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

/* manejador: rutina de tratamiento de la señal SIGINT. */
void manejador(int sig) {
    printf("Señal número %d recibida \n", sig);
    fflush(stdout);
}

int main(void) {
    int i;
    struct sigaction act;

    act.sa_handler = manejador;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;

    for (i = 1; i < 32; i++) {
	    if (sigaction(i, &act, NULL) < 0) {
	        perror("sigaction");
	        printf("%d\n", i);
	        fflush(stdout);
	        //exit(EXIT_FAILURE);
	    }
	}

    while(1) {
        printf("En espera de Ctrl+C (PID = %d)\n", getpid());
        sleep(9999);
    }
}
