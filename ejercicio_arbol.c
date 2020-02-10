#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_PROC 5

int main(int argc, char** argv) {
    int i;
    pid_t f;

    for (i = 0; i <= NUM_PROC; i++) {
        f = fork();
        if (f < 0) {
            return 1;
        }
        if (f > 0) {
            break;
        }
    }

    wait(NULL);
    return 0;
}