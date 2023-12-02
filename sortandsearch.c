#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int comparison(const void *a, const void *b)
{
	return (int)(*(int *)a - *(int *)b);
}

int main(int argc, char **args)
{
	int *buffer = NULL;
	char *rest = NULL;
	unsigned long len = 0;
	unsigned int i = 0;
	int key = 0;

	if (argc != 2) {
		perror("need to type len to generate random-data-buffer\n");
		exit(EXIT_FAILURE);
	}

	len = strtoul(args[1], &rest, 10);
	if (rest == args[1]) {
		perror("strtoul failed\n");
		exit(EXIT_FAILURE);
	}

	if (len > 256) {
		perror("max len is 256\n");
		exit(EXIT_FAILURE);
	}

	buffer = (int *)reallocarray(NULL, len, sizeof(int));
	if (!buffer) {
		perror("reallocarray failed\n");
		exit(EXIT_FAILURE);
	}

	memset(buffer, 0, len * sizeof(int));

////////////////////////////////////////////// generate random numbers ////////////////////////////////

	if (getentropy(buffer, len * sizeof(int))) {
		perror("getentropy failed\n");
		exit(EXIT_FAILURE);
	}

	for(i = 0; i < (unsigned int)len; i++) {
		if (i == 0) {
			//take a sample
			key = buffer[i];
		}
		printf("%d ", buffer[i]);
	}
	printf("\n");

////////////////////////////////////////////// quick sort ///////////////////////////////////////////////

	/*Quick Sort*/
	qsort(buffer, len, sizeof(int), comparison);

	for(i = 0; i < len; i++) {
		printf("%d ", buffer[i]);
	}
	printf("\n");

////////////////////////////////////////////// binary search ////////////////////////////////////////////

	/*If there are multiple elements that match the key, the element returned is unspecified*/
	/*Note that, compared elements could be anything like a struct. All needed is to pass suitable key to the comprarator function*/
	if (NULL == (int *)bsearch(&key, buffer, len, sizeof(int), comparison)) {
		printf("%d NOT found\n", key);
	} else {
		printf("%d found\n", key);
	}

	free(buffer);

	return EXIT_SUCCESS;
}
