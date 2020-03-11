CC = gcc
CFLAGS = -Wall

.PHONY: clean

all: ejercicio_kill ejercicio_prottemp

# ENLAZADO #
ejercicio_kill: ejercicio_kill.o
	$(CC) -o $@ $^

ejercicio_prottemp: ejercicio_prottemp.o
	$(CC) -o $@ $^

# COMPILADO #
ejercicio_kill.o: ejercicio_kill.c
	$(CC) $(CFLAGS) -c $< -o $@

ejercicio_prottemp.o: ejercicio_prottemp.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf *.o