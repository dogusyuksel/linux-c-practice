#define _GNU_SOURCE //vasprintf
#define _POSIX_C_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <stdbool.h>

static struct option parameters[] = {
	{"reader",		no_argument,		0,		'r'},
	{"writer",		no_argument,		0,		'w'},
	{"help",		no_argument,		0,		'h'},
	{NULL,			0,					0,		0}
};

void print_help(void)
{
	printf("type \"reader\" or 'r' to start fifo reader\n");
	printf("and use \"write\" or 'w' for writer\n");
	printf("Note that: no option is used for reader\n");
	exit(1);
}

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

int main(int argc, char **argv)
{
	int fd_read = 0, fd_write = 0;
	int option = 0, longid = 0;
	bool is_writer = false;
	char buffer[100] = {0};

	while ((option = getopt_long(argc, argv, "hrw", parameters, &longid)) != -1) {
		switch (option) {
			case 'h':
				// "optarg" variable is used to get option itself
				print_help();
				break;
			case 'r':
				is_writer = false;
				break;
			case 'w':
				is_writer = true;
				break;
			default:
				perror("unknown argument\n");
				return 2;
		}
	}

	mkfifo("myfifo", 0666);

	if (!is_writer) {
		//it is reader
		//since both ends should open the fifo simultaneously
		//O_NONBLOCK options is meaningless here
		fd_read = open("myfifo", O_RDONLY);
		if (fd_read < 0) {
			perror("open failed\n");
			return 2;
		}

		read(fd_read, buffer, sizeof(buffer));
		printf("reader reads \"%s\"\n", buffer);

		memset(buffer, 0, sizeof(buffer));

		close(fd_read);
		unlink("myfifo");

		return 0;
	}

	//it is writer
	//since both ends should open the fifo simultaneously
	//O_NONBLOCK options is meaningless here
	fd_write = open("myfifo", O_WRONLY);
	if (fd_write < 0) {
		perror("open failed\n");
		return 2;
	}

	strncpy(buffer, "hello from writer", sizeof(buffer));

	write(fd_write, buffer, sizeof(buffer));
	printf("writer writes \"%s\"\n", buffer);

	memset(buffer, 0, sizeof(buffer));

	close(fd_write);
	unlink("myfifo");

	return 0;
}
