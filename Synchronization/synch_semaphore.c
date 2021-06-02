#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define ITER 1000
#define BUFFER_SIZE 30

void* thread_increment(void* arg);
void* thread_decrement(void* arg);
int x;



/*
not_full: remaining data semaphore
not_empty: remaining space semaphore
mutex: critical section enterence binary semaphore
*/
sem_t not_empty;
sem_t not_full;
sem_t mutex;

int main() {
	pthread_t tid1, tid2;

	// semaphore initialization­
	sem_init(&not_empty, 0, 0);
	sem_init(&mutex, 0, 1);
	sem_init(&not_full, 0, BUFFER_SIZE - 1);


	pthread_create(&tid1, NULL, thread_increment, NULL);
	pthread_create(&tid2, NULL, thread_decrement, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	if (x != 0)
		printf("BOON! counter=%d\n", x);
	else
		printf("OK counter=%d\n", x);

	sem_destroy(&not_empty);
	sem_destroy(&not_full);
	sem_destroy(&mutex);

	return 0;
}

// producer function
void* thread_increment(void* arg) {
	int i, val;
	for (i = 0; i < ITER; i++) {
		sem_wait(&not_full);
		sem_wait(&mutex);

		// critical section begin
		val = x;
		printf("%u: %d\n", (unsigned int)pthread_self(), val);
		x = val + 1;
		// critical section end

		sem_post(&mutex);
		sem_post(&not_empty);
	}
	return NULL;
}

// consumer function
void* thread_decrement(void* arg) {
	int i, val;
	for (i = 0; i < ITER; i++) {
		sem_wait(&not_empty);
		sem_wait(&mutex);

		// critical section begin
		val = x;
		printf("%u: %d\n", (unsigned int)pthread_self(), val);
		x = val - 1;
		// critical section end

		sem_post(&mutex);
		sem_post(&not_full);
	}
	return NULL;
}
