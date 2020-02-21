/**
 * Fichero: ejercicio_shell_spawn.c
 *
 * Autores: Leandro Garcia (leandro.garcia@estudiante.uam.es)
 *          Fabian Gutierrez (fabian.gutierrez@estudiante.uam.es)
 * Grupo: 2201
 * Fecha: 15/02/2020
 * Descripcion: Implementa una shell rudimentaria (con spawn).
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wordexp.h>
#include <spawn.h>

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

		s[strcspn(s, "\n")] = '\0'; //removing newline

		if ( (error = wordexp(s, &exp, 0)) ) {
			fprintf(stderr, "wordexp: %s, %d\n", strerror(error), error);
			exit(EXIT_FAILURE);
		}

		error = posix_spawnp(NULL, exp.we_wordv[0], NULL, NULL, exp.we_wordv, NULL);

		if (error < 0) {
			fprintf(stderr, "posix_spawnp: %s\n", strerror(error));
			wordfree(&exp);
			exit(EXIT_FAILURE);
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
