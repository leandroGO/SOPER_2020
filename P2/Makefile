CC = gcc
CFLAGS = -Wall

.PHONY: clean

all: ejercicio_kill

# ENLAZADO #
ejercicio_kill: ejercicio_kill.o
	$(CC) -o $@ $^

# COMPILADO #
ejercicio_kill.o: ejercicio_kill.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf *.o