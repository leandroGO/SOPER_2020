##########################################
CC = gcc
CFLAGS = -Wall -ansi -pedantic

.PHONY: clean

# Enlazado #
ejercicio_hilos: ejercicio_hilos.o
	$(CC) -o $@ $^ -pthread

ejercicio_arbol: ejercicio_arbol.o
	$(CC) -o $@ $^

ejercicio_shell: ejercicio_shell.o
	$(CC) -o $@ $^

# Compilacion #
ejercicio_hilos.o: ejercicio_hilos.c
	$(CC) $(CFLAGS) -c $< -o $@ -pthread

ejercicio_arbol.o: ejercicio_arbol.c
	$(CC) $(CFLAGS) -c $< -o $@

ejercicio_shell.o: ejercicio_shell.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o
##########################################