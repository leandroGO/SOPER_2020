CC = gcc
CFLAGS = -Wall

.PHONY: clean

all: ejercicio_kill ejercicio_prottemp ejercicio_prottemp_mejorado ejercicio_prottemp_mejorado_op ejercicio_lect_escr

# ENLAZADO #
ejercicio_kill: ejercicio_kill.o
	$(CC) -o $@ $^

ejercicio_prottemp: ejercicio_prottemp.o
	$(CC) -o $@ $^

ejercicio_prottemp_mejorado: ejercicio_prottemp_mejorado.o
	$(CC) -o $@ $^ -pthread

ejercicio_prottemp_mejorado_op: ejercicio_prottemp_mejorado_op.o
	$(CC) -o $@ $^ -pthread

ejercicio_lect_escr: ejercicio_lect_escr.o
	$(CC) -o $@ $^ -pthread

# COMPILADO #
ejercicio_kill.o: ejercicio_kill.c
	$(CC) $(CFLAGS) -c $< -o $@

ejercicio_prottemp.o: ejercicio_prottemp.c
	$(CC) $(CFLAGS) -c $< -o $@

ejercicio_prottemp_mejorado.o: ejercicio_prottemp_mejorado.c
	$(CC) $(CFLAGS) -c $< -o $@

ejercicio_prottemp_mejorado_op.o: ejercicio_prottemp_mejorado_op.c
	$(CC) $(CFLAGS) -c $< -o $@

ejercicio_lect_escr.o: ejercicio_lect_escr.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf *.o