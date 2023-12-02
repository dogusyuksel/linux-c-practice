/*
There are two basic sorts of semaphores: binary semaphores, which never take on
values other than zero or one, and counting semaphores, which can take on
arbitrary nonnegative values.
A binary semaphore is logically just like a mutex.
Counting semaphore is used for multi process systems for example.

OUTPUT: (while sem initial value is 1)
//it works like mutexes

Entered..

Just Exiting...

Entered..

Just Exiting...

Entered..

Just Exiting...

OUTPUT: (while sem initial value is 2)
//it works like 2-core

Entered..

Entered..

Just Exiting...

Entered..

Just Exiting...

Just Exiting...
*/


#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
  
sem_t sem;
  
void *thread(void* arg)
{
	if (arg) {
		printf("arg: %p\n", arg);
	}

	//wait
	sem_wait(&sem); //decrement semaphore value and waits until the value is equal to zero
	printf("\nEntered..\n");

	//critical section
	sleep(4);
	
	//signal
	printf("\nJust Exiting...\n");
	sem_post(&sem);

	return NULL;
}

int main(void)
{
	pthread_t t1, t2, t3;
	pthread_attr_t thread_attr;

	//0 means not shared, only in same process
	//1 means semaphores initial value (it is binary semaphore)
	sem_init(&sem, 0, 1);

	if (pthread_attr_init(&thread_attr) != 0 ) {
		perror("pthread_attr_init failure.\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED) != 0 ) {
		perror("pthread_attr_setdetachstate failure.\n");
		exit(EXIT_FAILURE);
	}

	pthread_create(&t1, &thread_attr, thread, NULL);

	sleep(2);

	pthread_create(&t2, &thread_attr, thread, NULL);

	sleep(2);

	pthread_create(&t3, &thread_attr, thread, NULL);

	sem_destroy(&sem);

	//wait to see all threads are finished
	sleep(30);

	exit(EXIT_SUCCESS);
}
