#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define ITER 1000
#define BUFFER_SIZE 30

void* thread_increment(void* arg);
void* thread_decrement(void* arg);
int x;

/*
not_full: Remaining space condition variable
not_empty: Remaining data condition variable
mutex: critical section control mutex
*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

int main() {
	pthread_t tid1, tid2;

	pthread_create(&tid1, NULL, thread_increment, NULL);
	pthread_create(&tid2, NULL, thread_decrement, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	if (x != 0)
		printf("BOON! counter=%d\n", x);
	else
		printf("OK counter=%d\n", x);


}

// producer function
void* thread_increment(void* arg) {
	int i, val;
	for (i = 0; i < ITER; i++) {
		pthread_mutex_lock(&mutex);
		while (x == BUFFER_SIZE - 1) {
			pthread_cond_wait(&not_full, &mutex);
		}

		// critical section begin
		val = x;
		printf("%u: %d\n", (unsigned int)pthread_self(), val);
		x = val + 1;
		// critical section end
		pthread_cond_signal(&not_empty);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

// consumer function
void* thread_decrement(void* arg) {
	int i, val;
	for (i = 0; i < ITER; i++) {
		pthread_mutex_lock(&mutex);
		while (x == 0) {
			pthread_cond_wait(&not_empty, &mutex);
		}
		// critical section begin
		val = x;
		printf("%u: %d\n", (unsigned int)pthread_self(), val);
		x = val - 1;
		// critical section end

		pthread_cond_signal(&not_full);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
