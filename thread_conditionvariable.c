/*
The condition variable is used to signal changes in the variable’s state
The principal condition variable operations are signal and wait. The signal operation
is a notification to one or more waiting threads that a shared variable’s state has
changed. The wait operation is the means of blocking until such a notification is
received.
They are used with mutexes

While mutex implement synchronization by controlling thread access to data,
condition variables allow threads to synchronize based upon the actual value of data.

Without condition variables, the programmer would need to have threads continually
polling (possibly in a critical section), to check if the condition is met.

OUTPUT:
Waiting on condition variable
Signaling condition variable
Returning thread
Returning thread
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define TRUE	1
#define FALSE	0

#define RESPONSE_TIMEOUT_MS 5000

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int is_packet_received = FALSE;

void *receiver(void *args)
{
	if (args) {
		printf("args: %p\n", args);
	}

	pthread_mutex_lock(&lock);
	//read the packet and handle it
	//then inform the sender again to send other chunks
	is_packet_received = TRUE;
	pthread_cond_signal(&cond1);
	pthread_mutex_unlock(&lock);

	printf("Returning receiver thread\n");

	return NULL;
}

void *sender(void *args)
{
	struct timespec ts;

	if (args) {
		printf("args: %p\n", args);
	}

	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += (RESPONSE_TIMEOUT_MS / 1000);

	int is_packet_received_copy = FALSE;

	pthread_mutex_lock(&lock);
	if (is_packet_received == FALSE) {
		pthread_cond_timedwait(&cond1, &lock, &ts);
	}
	is_packet_received_copy = is_packet_received;
	is_packet_received = FALSE;
	pthread_mutex_unlock(&lock);

	if (is_packet_received_copy == FALSE) {
		printf("timeout, retry\n");
	} else {
		printf("packet received successfully\n");
	}

	printf("Returning sender thread\n");

	return NULL;
}

int main()
{
	pthread_t tid1, tid2;

	pthread_create(&tid1, NULL, receiver, NULL);

	// sleep for 1 sec so that thread 1
	// would get a chance to run first
	sleep(1);

	pthread_create(&tid2, NULL, sender, NULL);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	return 0;
}
