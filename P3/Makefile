CC = gcc
CFLAGS = -Wall

.PHONY: clean

all: shm_concurrence shm_concurrence_solved shm_producer shm_consumer shm_producer_file shm_consumer_file mq_injector mq_workers_pool mq_workers_pool_timed

# ENLAZADO #
shm_concurrence: shm_concurrence.o
	$(CC) -o $@ $^ -lrt -lm

shm_concurrence_solved: shm_concurrence_solved.o
	$(CC) -o $@ $^ -lrt -lm -pthread

shm_producer: shm_producer.o
	$(CC) -o $@ $^ -pthread -lrt

shm_consumer: shm_consumer.o
	$(CC) -o $@ $^ -pthread -lrt

shm_producer_file: shm_producer_file.o
	$(CC) -o $@ $^ -pthread -lrt

shm_consumer_file: shm_consumer_file.o
	$(CC) -o $@ $^ -pthread -lrt

mq_injector: mq_injector.o
	$(CC) -o $@ $^ -lrt

mq_workers_pool: mq_workers_pool.o
	$(CC) -o $@ $^ -lrt

mq_workers_pool_timed: mq_workers_pool_timed.o
	$(CC) -o $@ $^ -lrt

# COMPILADO #
shm_concurrence.o: shm_concurrence.c
	$(CC) $(CFLAGS) -c $< -o $@

shm_concurrence_solved.o: shm_concurrence_solved.c
	$(CC) $(CFLAGS) -c $< -o $@

shm_producer.o: shm_producer.c shm_producer_consumer.h
	$(CC) $(CFLAGS) -c $< -o $@

shm_consumer.o: shm_consumer.c shm_producer_consumer.h
	$(CC) $(CFLAGS) -c $< -o $@

shm_producer_file.o: shm_producer_file.c shm_producer_consumer.h
	$(CC) $(CFLAGS) -c $< -o $@

shm_consumer_file.o: shm_consumer_file.c shm_producer_consumer.h
	$(CC) $(CFLAGS) -c $< -o $@

mq_injector.o: mq_injector.c
	$(CC) $(CFLAGS) -c $< -o $@

mq_workers_pool.o: mq_workers_pool.c
	$(CC) $(CFLAGS) -c $< -o $@

mq_workers_pool_timed.o: mq_workers_pool_timed.c
	$(CC) $(CFLAGS) -c $< -o $@

# MISC #
clean:
	rm -rf *.o