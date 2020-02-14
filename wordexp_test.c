#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wordexp.h>

int main(int argc, char **argv)   {
	wordexp_t p;
	char **w;
	int i;
	wordexp("ls", &p, 0);
	w = p.we_wordv;
	for (i = 0; i < p.we_wordc; i++)
		printf("%s ", w[i]);
	printf("\nSalida:\n");
	if(!fork()) {
		if (execvp(p.we_wordv[0], p.we_wordv)) {
			perror("ERROR");
			wordfree(&p);
			exit(EXIT_FAILURE);
		}
	}
	wait(NULL);
	wordfree(&p);
	exit(EXIT_SUCCESS);
}