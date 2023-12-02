#define _GNU_SOURCE //vasprintf
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

static pid_t childpid;
static int	writepipe[2] = {-1,-1},	readpipe [2] = {-1,-1};

#define	PARENT_READ	readpipe[0] // read and write pipes from the point of view of parent
#define	CHILD_WRITE	readpipe[1]
#define CHILD_READ	 writepipe[0]
#define PARENT_WRITE	writepipe[1]

static void print_error_and_exit(const char *format, ...)
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

static void read_from_pipe(int fd)
{
	FILE *stream;
	int c;
	stream = fdopen (fd, "r"); // file descriptor to file stream conversion
	while ((c = fgetc (stream)) != EOF)
		putchar(c);
	fclose (stream);
}

static void collect_zombie(int sig)
{
	int ret;
	int status;

	fprintf(stderr, "caught signal: %d\n", sig);

	if (sig != SIGCHLD &&
		childpid != (ret = waitpid(childpid, &status, WNOHANG))) {
		print_error_and_exit("waitpid() failedwith %d", ret);
	}
}

int main(void)
{
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);

	sa.sa_flags = 0;
	sa.sa_handler = collect_zombie;

	sigaction(SIGCHLD, &sa, NULL);

	if (pipe(writepipe) != 0) {
		print_error_and_exit("pipe() failed");
	}
	if (pipe(readpipe) != 0) {
		print_error_and_exit("pipe() failed");
	}

	if ((childpid = fork()) < 0) {
		print_error_and_exit("fork() failed");
	} else if ( childpid == 0 )	{
		/* in order for the parent to send data,
		it should remain open on only parent side
		*/
		close(PARENT_WRITE);
		/* in order to send data to parent,
		this port remains open on only parent side
		*/
		close(PARENT_READ);
		/* child makes STDIN the read side of the pipe.
		So the data send by PARENT_WRITE will be gathered by child over STDIN
		*/
		dup2(CHILD_READ,  STDIN_FILENO);
		/* should be closed because we already duplicated
		*/
		close(CHILD_READ);
		/* child makes STDOUT the write side of the pipe.
		It means, the data send over CHILD_WRITE will be seen on PARENT_READ on parent side
		*/
		dup2(CHILD_WRITE, STDOUT_FILENO);
		/* should be closed because we already duplicated
		*/
		close(CHILD_WRITE);

		/* do child stuff */
		char *command[] = {(char *)"echo", "hello", 0};
		char *bin_file = command[0];

		if (execvp(bin_file, command) == -1) {
		  print_error_and_exit("execvp() failed");
		}

		while(1); // never reach here
	} else {
		/* in order to send data to the child, it must remain open only on child side
		*/
		close(CHILD_READ);
		/* in order to read from child, it must remain open on only child side
		*/
		close(CHILD_WRITE);

		/* do parent stuff */
		read_from_pipe(PARENT_READ);
	}

	sleep(10);
	close(PARENT_WRITE);
	close(PARENT_READ);
}
