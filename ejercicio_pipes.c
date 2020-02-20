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

	if (pipe(fd1) == -1) {
		perror("pipe");
		return EXIT_FAILURE;
	}

	h1 = fork();
	if (h1 == -1) {
		perror("fork");
		close(fd1[1]);
		close(fd1[0]);
		return EXIT_FAILURE;
	}

	else if (h1 > 0) {
		close(fd1[1]);

		if (pipe(fd2) == -1) {
			perror("pipe");
			close(fd1[0]);
			return EXIT_FAILURE;
		}

		h2 = fork();
		if (h2 == -1) {
			perror("fork");
			close(fd1[0]);
			close(fd2[0]);
			close(fd2[1]);
			return EXIT_FAILURE;
		}
		else if (h2 > 0) {
			/*Padre*/
			close(fd2[0]);

			if (read(fd1[0], &x, sizeof(int)) < sizeof(int)) {
				perror("read");
				close(fd1[0]);
				close(fd2[1]);
				return EXIT_FAILURE;
			}

			if (write(fd2[1], &x, sizeof(int)) < sizeof(int)) {
				perror("write");
				close(fd1[0]);
				close(fd2[1]);
				return EXIT_FAILURE;
			}

			close(fd1[0]);
			close(fd2[1]);
		}
		else {
			/*Hijo 2*/
			close(fd1[0]);
			close(fd2[1]);

			if (read(fd2[0], &x, sizeof(int)) < sizeof(int)) {
				perror("read");
				close(fd2[0]);
				return EXIT_FAILURE;
			}

			f = open("numero_leido.txt", O_RDWR | O_CREAT |O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR | S_IROTH | S_IWOTH | S_IXOTH);
			if (f < 0) {
				perror("open");
				close(fd2[0]);
				return EXIT_FAILURE;
			}

			sprintf(buf, "%d\n", x);

			if (write(f, buf, strlen(buf)) < strlen(buf)) {
				perror("write");
				close(fd2[0]);
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

		srand((unsigned) time(NULL));
		x = rand();

		if (write(fd1[1], &x, sizeof(int)) < sizeof(int)) {
			perror("write");
			close(fd1[1]);
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