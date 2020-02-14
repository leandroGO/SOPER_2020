#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wordexp.h>

#define BUF 64

int main(int argc, char *argv[]) {
	int error = 0;
	wordexp_t exp;
	char s[BUF] = "";

	while(1) {
		if (fprintf(stdout, ">> ") < 0) {
			perror("Error printing");
			exit(EXIT_FAILURE);
		}

		if (fgets(s, BUF, stdin) == NULL) {	/*stdin: EOF*/
			break;
		}

		if ( (error = wordexp(s, &exp, 0)) ) {
			fprintf(stderr, "wordexp: %s\n", strerror(error));
			exit(EXIT_FAILURE);
		}

		if (!fork()) {
			execvp(exp.we_wordv[0], exp.we_wordv);
		}
		wait(NULL);
		wordfree(&exp);
	}

	fprintf(stdout, "\n");
	exit(EXIT_SUCCESS);
}