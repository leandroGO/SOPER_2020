#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "shm_producer_consumer.h"

int main(int argc, char *argv[]) {
    int N, rand;

    if (argc != 3 || (N = atoi(argv[1])) < 0 || (rand = atoi(argv[2])) < 0 || rand > 1) {
        printf("ERROR: deberia ser %s <N> <rand> (con N no negativo y rand 0 o 1)\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}