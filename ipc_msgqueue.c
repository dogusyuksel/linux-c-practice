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
#include <sys/ipc.h>
#include <sys/msg.h>

static struct option parameters[] = {
	{"reader",		no_argument,		0,		'r'},
	{"writer",		no_argument,		0,		'w'},
	{"help",		no_argument,		0,		'h'},
	{NULL,			0,					0,		0}
};

void print_help(void)
{
	printf("type \"reader\" or 'r' to start queue reader\n");
	printf("and use \"write\" or 'w' for writer\n");
	printf("Note that: no option is used for reader\n");
	exit(1);
}

int main(int argc, char **argv)
{
	int option = 0, longid = 0;
	bool is_writer = false;
	char buffer[100] = {0};
	key_t key;
	char proj_id = 'D';
	int msqid;

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

	//create the file first
	FILE *fs = fopen("msgqueue", "r");
	if (!fs) {
		fs = fopen("msgqueue", "a+");
		if (!fs)
			perror("fopen");
		
		fclose(fs);
	}

	if ((key = ftok("msgqueue", proj_id)) == -1) {
		perror("ftok");
		exit(1);
	}

	//connect to the queue
	if ((msqid = msgget(key, 0666 | IPC_CREAT)) == -1) {
		perror("msgget");
		exit(1);
	}

	if (!is_writer) {
		//it is reader
		if (msgrcv(msqid, buffer, sizeof(buffer), 0, 0) == -1) {
			perror("msgrcv");
			exit(1);
		}
		printf("recvd: \"%s\"\n", buffer);

		return 0;
	}

	//it is writer
	if (msgsnd(msqid, "hello from writer", 18, 0) == -1)
		perror("msgsnd");
		
	if (msgctl(msqid, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(1);
	}

	unlink("msgqueue");

	return 0;
}
