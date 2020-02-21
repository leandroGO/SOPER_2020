/**
 * Fichero: ejercicio_shell.c
 *
 * Autores: Leandro Garcia (leandro.garcia@estudiante.uam.es)
 *          Fabian Gutierrez (fabian.gutierrez@estudiante.uam.es)
 * Grupo: 2201
 * Fecha: 15/02/2020
 * Descripcion: Implementa una shell rudimentaria.
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wordexp.h>

#define BUF 64

int main(int argc, char *argv[]) {
	int error = 0, wstatus = 0;
	wordexp_t exp;
	char s[BUF] = "";

	while(1) {
		if (fprintf(stdout, ">> ") < 0) {
			perror("Error printing");
			exit(EXIT_FAILURE);
		}

		if (fgets(s, BUF, stdin) == NULL) {	//stdin: EOF
			break;
		}

		s[strcspn(s, "\n")] = '\0'; //eliminando newline

		if ( (error = wordexp(s, &exp, 0)) ) {
			fprintf(stderr, "wordexp: %s\n", strerror(error));
			exit(EXIT_FAILURE);
		}

		if ((error = fork()) < 0) {
			fprintf(stderr, "fork: %s\n", strerror(error));
			wordfree(&exp);
			exit(EXIT_FAILURE);
		}

		if (!error) {
			execvp(exp.we_wordv[0], exp.we_wordv);
		}

		wait(&wstatus);
		if (WIFEXITED(wstatus)) {
			fprintf(stderr, "Exited with value %d\n", WEXITSTATUS(wstatus));
		}
		else if (WIFSIGNALED(wstatus)) {
			fprintf(stderr, "Terminated by signal %d\n", WTERMSIG(wstatus));
		}
		wordfree(&exp);
	}

	fprintf(stdout, "\n");
	exit(EXIT_SUCCESS);
}
