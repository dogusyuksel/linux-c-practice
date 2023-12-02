#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>

int sum(int num_args, ...)
{
	int val = 0;
	va_list ap;
	int i;

	va_start(ap, num_args);
	for(i = 0; i < num_args; i++) {
		val += va_arg(ap, int);
	}
	va_end(ap);

	return val;
}

int main(void)
{
	printf("Sum of 15 and 56 = %d\n", sum(2, 15, 56));

	exit(EXIT_SUCCESS);
}