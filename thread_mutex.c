
/*
Protecting Accesses to Shared Variables
A mutex has two states: locked and unlocked. At any moment, at most one thread
may hold the lock on a mutex. Attempting to lock a mutex that is already locked
either blocks or fails with an error, depending on the method used to place the lock.
Sometimes, a thread needs to simultaneously access two or more different shared
resources, each of which is governed by a separate mutex. When more than one
thread is locking the same set of mutexes, deadlock situations can arise. Figure 30-3
shows an example of a deadlock in which each thread successfully locks one mutex,
and then tries to lock the mutex that the other thread has already locked. Both
threads will remain blocked indefinitely.
The simplest way to avoid such deadlocks is to define a mutex hierarchy.
When threads can lock the same set of mutexes, they should always lock
them in the same order.
An alternative strategy that is less frequently used is “try, and then back off.”

OUTPUT:
passed data is 20
counter: 1 
passed data is 10
counter: 2 
returned retval 212531 from 993f6640
returned retval 212532 from 98bf5640

if not mutex then the output
passed data is 10
passed data is 20
counter: 2 
returned retval 212652 from 587a7640
counter: 2 
returned retval 212653 from 57fa6640
*/

#define _GNU_SOURCE //gettid
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int shared_counter;
pthread_mutex_t lock;

void *trythis(void *arg)
{
	char *buffer = NULL;

	pthread_mutex_lock(&lock);
		if ((int *)arg) {
			printf("passed data is %d\n", *(int *)arg);
		}
		shared_counter += 1;
		sleep(5);
		printf("counter: %d \n", shared_counter);

		if ((buffer = (char*) malloc(64)) == NULL) {
			perror("malloc() error");
			exit(EXIT_FAILURE);
		}

		snprintf(buffer, 64, "retval %d", gettid());
	pthread_mutex_unlock(&lock);

	return (buffer) ? buffer : NULL;
;
}

int main(void)
{
	pthread_t thread_id1, thread_id2;
	int passed_data1 = 10, passed_data2 = 20;
	char *retval_ptr = NULL;

	if (pthread_mutex_init(&lock, NULL) != 0) {
		perror("mutex init has failed\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&(thread_id1), NULL, &trythis, (void *)&passed_data1) != 0) {
		perror("error while creating the thread\n");
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&(thread_id2), NULL, &trythis, (void *)&passed_data2) != 0) {
		perror("error while creating the thread\n");
		exit(EXIT_FAILURE);
	}

	pthread_join(thread_id1, (void **)&retval_ptr);
	printf("returned %s from %x\n", retval_ptr, (int)thread_id1);
	if (retval_ptr) {
		free(retval_ptr);
	}

	pthread_join(thread_id2, (void **)&retval_ptr);
	printf("returned %s from %x\n", retval_ptr, (int)thread_id2);
	if (retval_ptr) {
		free(retval_ptr);
	}

	pthread_mutex_destroy(&lock);
	// note: if "PTHREAD_MUTEX_INITIALIZER" is used while initializing, then destroy is needed

	exit(EXIT_SUCCESS);
}
