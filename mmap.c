#define _GNU_SOURCE //vasprintf
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

void print_error_and_exit(const char *format, ...)
{
	va_list mlist;
	char *buffer = NULL;

	va_start(mlist, format);
	vasprintf(&buffer, format, mlist);
	va_end(mlist);

	if (!buffer)
		exit(1);

	fprintf(stderr, "%s\n", buffer);
	free(buffer);

	exit(2);
}

int main(void)
{
	int estimatedbuffersize = 100;
	char *buffer = NULL, *buff_start = NULL;
	int fd = open("mmapfile", O_CREAT | O_RDWR | O_NONBLOCK, 0666);

	ftruncate(fd, estimatedbuffersize);

	buffer = buff_start = mmap(NULL, estimatedbuffersize, PROT_WRITE, MAP_SHARED, fd, 0);
	if (!buffer) {
		close(fd);
		print_error_and_exit("mmap failed\n");
	}

	buffer[0] = 'a';
	buffer[1] = 's';
	buffer[2] = 'd';
	buffer[3] = '\n';

	msync(buff_start, estimatedbuffersize, MS_SYNC);

	munmap(buff_start, estimatedbuffersize);

	close(fd);

	//OUTPUT: cat mmapfile
	// asd


	return 0;
}
