#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT  9191

#define TRUE		1
#define FALSE		0

int main (int argc, char **args)
{
	int rc;
	int nfds = 1;
	struct pollfd fds[1];
	unsigned long timeout = 0;
	char *rest = NULL;
	struct timespec ts_start, ts_now;

	if (argc != 2) {
		perror("give arg for timeout\n");
		exit(EXIT_FAILURE);
	}

	fds[0].fd = 0;
	fds[0].events = POLLIN; //poll input events

	timeout = strtoul(args[1], &rest, 10);
	if (rest == args[1]) {
		perror("strtoul failed\n");
		exit(EXIT_FAILURE);
	}

	if (clock_gettime(CLOCK_BOOTTIME, &ts_start) == -1) {
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}

	while (TRUE) {
		printf("Waiting on poll()...\n");
		rc = poll(fds, nfds, timeout);
		if (rc < 0) {
			perror("  poll() failed");
			break;
		} else if (rc == 0) {
			printf("  poll() timed out.\n");

			if (clock_gettime(CLOCK_BOOTTIME, &ts_now) == -1) {
				perror("clock_gettime");
				exit(EXIT_FAILURE);
			}

			printf("elapsed time %lu\n", (unsigned long)(ts_now.tv_sec - ts_start.tv_sec));
			ts_start = ts_now;
		}
	}
}
