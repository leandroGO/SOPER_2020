#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "sort.h"

int main(int argc, char *argv[]) {
    int n_levels, n_processes, delay;
    time_t t1, t2;
    FILE *f;
    pid_t ppid = getpid();

    f = fopen("times.dat", "w");
    if (f == NULL) {
        perror("fopen");
        return 1;
    }

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <FILE> <N_LEVELS> <N_PROCESSES> [<DELAY>]\n", argv[0]);
        fprintf(stderr, "    <FILE> :        Data file\n");
        exit(EXIT_FAILURE);
    }

    n_levels = 10;
    delay = 1e8;
    for (n_processes = 1; n_processes < 10; n_processes++) {
        t1 = time(NULL);
        sort_multiprocess(argv[1], n_levels, n_processes, delay);
        t2 = time(NULL);
        fprintf(f, "%d %ld\n", n_processes, t2-t1);
        fprintf(stdout, "%d %ld\n", n_processes, t2-t1);
    }


    return 1;
}
