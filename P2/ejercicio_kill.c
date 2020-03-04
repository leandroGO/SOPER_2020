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