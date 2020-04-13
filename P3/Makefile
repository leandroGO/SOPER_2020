CC = gcc
CFLAGS = -Wall

.PHONY: clean

all: shm_concurrence shm_producer shm_consumer shm_producer_file shm_consumer_file

# ENLAZADO #
shm_concurrence: shm_concurrence.o
	$(CC) -o $@ $^ -lrt -lm

shm_producer: shm_producer.o
	$(CC) -o $@ $^ -pthread -lrt

shm_consumer: shm_consumer.o
	$(CC) -o $@ $^ -pthread -lrt

shm_producer_file: shm_producer_file.o
	$(CC) -o $@ $^ -pthread -lrt

shm_consumer_file: shm_consumer_file.o
	$(CC) -o $@ $^ -pthread -lrt

# COMPILADO #
shm_concurrence.o: shm_concurrence.c
	$(CC) $(CFLAGS) -c $< -o $@

shm_producer.o: shm_producer.c shm_producer_consumer.h
	$(CC) $(CFLAGS) -c $< -o $@

shm_consumer.o: shm_consumer.c shm_producer_consumer.h
	$(CC) $(CFLAGS) -c $< -o $@

shm_producer_file.o: shm_producer_file.c shm_producer_consumer.h
	$(CC) $(CFLAGS) -c $< -o $@

shm_consumer_file.o: shm_consumer_file.c shm_producer_consumer.h
	$(CC) $(CFLAGS) -c $< -o $@

# MISC #
clean:
	rm -rf *.o