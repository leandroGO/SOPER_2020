##########################################
CC = gcc
CFLAGS = -Wall -ansi -pedantic

.PHONY: clean

ejercicio_arbol: ejercicio_arbol.o
	$(CC) -o $@ $^

ejercicio_arbol.o: ejercicio_arbol.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o
##########################################