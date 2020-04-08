CC = gcc
CFLAGS = -Wall

.PHONY: clean

all: shm_producer shm_consumer

# ENLAZADO #
shm_producer: shm_producer.o
	$(CC) -o $@ $^ -pthread -lrt

shm_consumer: shm_consumer.o
	$(CC) -o $@ $^ -pthread -lrt

# COMPILADO #
shm_producer.o: shm_producer.c shm_producer_consumer.h
	$(CC) $(CFLAGS) -c $< -o $@

shm_consumer.o: shm_consumer.c shm_producer_consumer.h
	$(CC) $(CFLAGS) -c $< -o $@

# MISC #
clean:
	rm -rf *.o