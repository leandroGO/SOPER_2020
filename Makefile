##########################################
CC = gcc
CFLAGS = -Wall

.PHONY: clean

# Enlazado #
ejercicio_hilos: ejercicio_hilos.o
	$(CC) -o $@ $^ -pthread

ejercicio_arbol: ejercicio_arbol.o
	$(CC) -o $@ $^

ejercicio_shell: ejercicio_shell.o
	$(CC) -o $@ $^

ejercicio_shell_spawn: ejercicio_shell_spawn.o
	$(CC) -o $@ $^

ejercicio_pipes: ejercicio_pipes.o
	$(CC) -o $@ $^

foo: foo.o
	$(CC) -o $@ $^

# Compilacion #
ejercicio_hilos.o: ejercicio_hilos.c
	$(CC) $(CFLAGS) -c $< -o $@ -pthread

ejercicio_arbol.o: ejercicio_arbol.c
	$(CC) $(CFLAGS) -c $< -o $@

ejercicio_shell.o: ejercicio_shell.c
	$(CC) $(CFLAGS) -c $< -o $@

ejercicio_shell_spawn.o: ejercicio_shell_spawn.c
	$(CC) $(CFLAGS) -c $< -o $@

ejercicio_pipes.o: ejercicio_pipes.c
	$(CC) $(CFLAGS) -c $< -o $@

foo.o: foo.c
	$(CC) $(CFLAGS) -c $< -o $@

# Misc #
clean:
	rm -rf *.o
##########################################