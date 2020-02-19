#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char** argv) {
	pid_t h1, h2;
	int fd1[2], fd2[2], f;
	int x = 0;

	if (pipe(fd1) == -1) {
		perror("pipe");
		return EXIT_FAILURE;
	}

	h1 = fork();
	if (h1 == -1) {
		perror("fork");
		return EXIT_FAILURE;
	}

	else if (h1 > 0) {
		close(fd1[1]);

		if (pipe(fd2) == -1) {
			perror("pipe");
			return EXIT_FAILURE;
		}

		h2 = fork();
		if (h2 == -1) {
			perror("fork");
			return EXIT_FAILURE;
		}
		else if (h2 > 0) {
			/*Padre*/
			close(fd2[0]);

			if (read(fd1[0], &x, sizeof(int)) < sizeof(int)) {
				perror("read");
				return EXIT_FAILURE;
			}

			if (write(fd2[1], &x, sizeof(int)) < sizeof(int)) {
				perror("write");
				return EXIT_FAILURE;
			}

			close(fd1[0]);
			close(fd2[1]);
		}
		else {
			/*Hijo 2*/
			close(fd2[1]);

			if (read(fd2[0], &x, sizeof(int)) < sizeof(int)) {
				perror("read");
				return EXIT_FAILURE;
			}

			f = open("numero_leido.txt", O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR | S_IROTH | S_IWOTH | S_IXOTH);
			if (f < 0) {
				perror("open");
				return EXIT_FAILURE;
			}

			if (write(f, &x, sizeof(int)) < sizeof(int)) {
				perror("write");
				return EXIT_FAILURE;
			}

			close(fd2[0]);
			close(f);
			return EXIT_SUCCESS;
		}
	}

	else {
		/*Hijo 1*/
		close(fd1[0]);

		x = rand();

		if (write(fd1[1], &x, sizeof(int)) < sizeof(int)) {
			perror("write");
			return EXIT_FAILURE;
		}

		printf("Valor de x: %d\n", x);
		close(fd1[1]);
		return EXIT_SUCCESS;
	}

	waitpid(h1, NULL, 0);
	waitpid(h2, NULL, 0);
	return EXIT_SUCCESS;
}