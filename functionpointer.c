#define _GNU_SOURCE //vasprintf
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int execute_callback(int (*callback_funct)(int), int data)
{
	if (!callback_funct)
		return 0;
	return callback_funct(data);
}

int my_callback(int data)
{
	printf("data: %d\n", data);
	return 0;
}

int main(void)
{
	execute_callback(&my_callback, 10);
	return 0;
}
