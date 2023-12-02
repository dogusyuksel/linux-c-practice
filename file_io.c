#define _GNU_SOURCE //vasprintf
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

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

	//%d, %i		--> signed int
	//%u			--> unsigned int
	//%hi			--> signed short
	//%hu			--> unsigned short
	//%l, %ld, %li	--> signed long
	//%lu			--> unsigned long
	//%lld			--> signed long long
	//%llu			--> unsigned long long
	//%f			--> float
	//%lf			--> double
	//%Lf			--> long double
	//%8d			--> add space
	//%08d			--> add zero
	//%0.2f			--> specify precision

	exit(2);
}

int main(void)
{
	int flag = 0;
	int fd = open("legacyfile", O_CREAT | O_RDWR | O_NONBLOCK, 0666);

	// O_CREATE
	// O_RDWR
	// O_RDONLY
	// O_WRONLY
	// O_NONBLOCK

	// MSB				LSB
	// 0		0		0		--> execute
	// 0		1		0		--> write
	// 1		0		0		--> read

	ftruncate(fd, 0);

	if (fd < 0)
		print_error_and_exit("open failed\n");
	
	if ((flag = fcntl(fd, F_GETFL)) < 0) {
		close(fd);
		print_error_and_exit("fcntl error\n");
	}
	printf("fcntl: %x\n", flag);

	write(fd, "3 selam1\n", 9);
	write(fd, "5 selam2\n", 9);

	fsync(fd);

	lseek(fd, 0, SEEK_SET);

	char sp[100] = {0};
	read(fd, sp, sizeof(sp));

	close(fd);

	printf("%s", sp);

	int base = 10;
	long value = 0;
	char *endptr = NULL;

	value = strtol(sp, &endptr, base);

	if (endptr == sp) {
		printf("value: %ld no valid digit\n", value);
	} else if (!endptr && endptr[0] != '\0') {
		printf("value: %ld but there are more\n", value);
	} else {
		printf("value: %ld and there is NO more\n", value);
	}

	//OUTPUT:
	// fcntl: 8802
	// 3 selam1
	// 5 selam2
	// value: 3 and there is NO more


	return 0;
}
