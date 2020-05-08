/**
 * Fichero: ejercicio_pipes.c
 *
 * Autores: Leandro Garcia (leandro.garcia@estudiante.uam.es)
 *          Fabian Gutierrez (fabian.gutierrez@estudiante.uam.es)
 * Grupo: 2201
 * Fecha: 20/02/2020
 * Descripcion: El proceso crea dos hijos usando fork y 
 * un dato aleatorio x que se pasa de uno a otro por medio de pipes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

int main(int argc, char** argv) {
	pid_t h1, h2;
	int fd1[2], fd2[2], f;
	int x = 0;
	char buf[50];

	buf[0] = '\0';

	/*Creacion del primer pipe*/
	if (pipe(fd1) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	/*Creacion del primer hijo*/
	h1 = fork();
	if (h1 == -1) {
		perror("fork");
		close(fd1[1]);
		close(fd1[0]);
		exit(EXIT_FAILURE);
	}

	else if (h1 == 0) {
		/*Hijo 1*/
		close(fd1[0]); //Cierre del extremo de lectura

		/*Creacion del numero aleatorio*/
		srand((unsigned) time(NULL));
		x = rand();

		/*Escritura en el pipe*/
		if (write(fd1[1], &x, sizeof(int)) < sizeof(int)) {
			perror("write");
			close(fd1[1]);
			exit(EXIT_FAILURE);
		}

		printf("Valor de x: %d\n", x);
		close(fd1[1]);
		exit(EXIT_SUCCESS);
	}
		
	close(fd1[1]); //Cierre del extremo de escritura

	/*Creacion del segundo pipe*/
	if (pipe(fd2) == -1) {
		perror("pipe");
		waitpid(h1, NULL, 0);
		close(fd1[0]);
		exit(EXIT_FAILURE);
	}

	/*Creacion del seguno hijo*/
	h2 = fork();
	if (h2 == -1) {
		perror("fork");
		waitpid(h1, NULL, 0);
		close(fd1[0]);
		close(fd2[0]);
		close(fd2[1]);
		exit(EXIT_FAILURE);
	}
	
	else if (h2 == 0) {
		/*Hijo 2*/
		close(fd1[0]); //Cierre del primer extremo de lectura
		close(fd2[1]); //Cierre del segundo extremo de escritura

		/*Lectura del segundo pipe*/
		if (read(fd2[0], &x, sizeof(int)) < sizeof(int)) {
			perror("read");
			close(fd2[0]);
			exit(EXIT_FAILURE);
		}
		
		/*Apertura de fichero y escritura*/
		f = open("numero_leido.txt", O_RDWR | O_CREAT |O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR |
																S_IROTH | S_IWOTH | S_IXOTH);
		if (f < 0) {
			perror("open");
			close(fd2[0]);
			exit(EXIT_FAILURE);
		}

		sprintf(buf, "%d\n", x);

		if (write(f, buf, strlen(buf)) < strlen(buf)) {
			perror("write");
			close(fd2[0]);
			exit(EXIT_FAILURE);
		}

		close(fd2[0]);
		close(f);
		exit(EXIT_SUCCESS);
	}	

	/*Padre*/
	close(fd2[0]); //Cierre del segundo extremo de lectura

	/*Lectura del primer pipe*/
	if (read(fd1[0], &x, sizeof(int)) < sizeof(int)) {
		perror("read");
		waitpid(h1, NULL, 0);
		waitpid(h2, NULL, 0);
		close(fd1[0]);
		close(fd2[1]);
		exit(EXIT_FAILURE);
	}

	/*Escritura en el segundo pipe*/
	if (write(fd2[1], &x, sizeof(int)) < sizeof(int)) {
		perror("write");
		waitpid(h1, NULL, 0);
		waitpid(h2, NULL, 0);
		close(fd1[0]);
		close(fd2[1]);
		exit(EXIT_FAILURE);
	}

	close(fd1[0]);
	close(fd2[1]);

	waitpid(h1, NULL, 0);
	waitpid(h2, NULL, 0);
	exit(EXIT_SUCCESS);
}