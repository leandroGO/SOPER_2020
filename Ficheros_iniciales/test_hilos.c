#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void * slow_printf(void * arg)
{
	const char * msg = arg;
	int error = 0;

	/*error = pthread_detach(pthread_self());
	if(error != 0)
	{
		fprintf(stderr, "pthread_detach: %s\n", strerror(error));
		pthread_exit(NULL);
	}*/

	for(size_t i = 0; i < strlen(msg); i++)
	{
		printf("%c ", msg[i]);
		fflush(stdout);
		sleep(1);
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t h1;
	pthread_t h2;
	char * hola = "Hola ";
	char * mundo = "Mundo";
	int error;

	error = pthread_create(&h1, NULL, slow_printf, hola);
	if(error != 0)
	{
		fprintf(stderr, "pthread_create: %s\n", strerror(error));
		pthread_exit(NULL);
	}

	error = pthread_create(&h2, NULL, slow_printf, mundo);
	if(error != 0)
	{
		fprintf(stderr, "pthread_create: %s\n", strerror(error));
		pthread_exit(NULL);
	}

	error = pthread_detach(h1);
	if(error != 0)
	{
		fprintf(stderr, "pthread_detach: %s\n", strerror(error));
		exit(EXIT_FAILURE);
	}

	error = pthread_detach(h2);
	if(error != 0)
	{
		fprintf(stderr, "pthread_detach: %s\n", strerror(error));
		exit(EXIT_FAILURE);
	}

	printf("El programa %s termino correctamente \n", argv[0]);
	/*sleep(8);*/
	pthread_exit(NULL);
}
