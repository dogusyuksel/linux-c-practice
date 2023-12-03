#define _GNU_SOURCE //vasprintf
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

static pid_t childpid;
static int	writepipe[2] = {-1,-1},	readpipe [2] = {-1,-1};
char **args_dup = NULL;
int argc_dup = 0;

#define WORDS_SIZE	32

#define	PARENT_READ	readpipe[0] // read and write pipes from the point of view of parent
#define	CHILD_WRITE	readpipe[1]
#define CHILD_READ	 writepipe[0]
#define PARENT_WRITE	writepipe[1]

static void dealloc_all(void)
{
	for (int i = 0; i < argc_dup; i++) {
		if (args_dup[i]) {
			free(args_dup[i]);
		}
	}

	if (args_dup) {
		free(args_dup);
	}
}

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

	dealloc_all();

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

static void collect_zombie(int sig, siginfo_t *siginfo, void *context)
{
	int ret;
	int status;

	if (context) {
		fprintf(stdout, "APP_TRACKER ==> context: %p\n", context);
	}

	fprintf(stderr, "APP_TRACKER ==> caught signal: %d\n", sig);

	if (sig != SIGCHLD) {
		return;
	}

	switch(siginfo->si_code)
	{
		case CLD_EXITED:
			fputs("APP_TRACKER ==> Caught SIGCHLD: (Child has exited)\n", stderr);
			break;
		case CLD_KILLED:
			fputs("APP_TRACKER ==> Caught SIGCHLD: (Child was killed)\n", stderr);
			break;
		case CLD_DUMPED:
			fputs("APP_TRACKER ==> Caught SIGCHLD: (Child terminated abnormally)\n", stderr);
			break;
		case CLD_TRAPPED:
			fputs("APP_TRACKER ==> Caught SIGCHLD: (Traced child has trapped)\n", stderr);
			break;
		case CLD_STOPPED:
			fputs("APP_TRACKER ==> Caught SIGCHLD: (Child has stopped)\n", stderr);
			break;
		case CLD_CONTINUED:
			fputs("APP_TRACKER ==> Caught SIGCHLD: (Stopped child has continued)\n", stderr);
			break;
		default:
			fputs("APP_TRACKER ==> Caught SIGCHLD: default\n", stderr);
			break;
	}

	if (childpid != (ret = waitpid(childpid, &status, WNOHANG))) {
		print_error_and_exit("APP_TRACKER ==> waitpid() failed with %d", ret);
		return;
	}

	if (WIFEXITED(status)) {
		fprintf(stderr, "APP_TRACKER ==> child is exited, status = %d\n", WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
		fprintf(stderr, "APP_TRACKER ==>  shiled is killed by signal %d\n", WTERMSIG(status));
	} else if (WIFSTOPPED(status)) {
		fprintf(stderr, "APP_TRACKER ==> child is stopped by signal %d\n", WSTOPSIG(status));
	} else if (WIFCONTINUED(status)) {
		fprintf(stderr, "APP_TRACKER ==> child is continued\n");
	}

	dealloc_all();

	exit(0);
}

int main(int argc, char **args)
{
	argc_dup = argc;
	struct sigaction sa;

	if (argc < 2) {
		print_error_and_exit("APP_TRACKER ==> at least one arg necessary to start the given application");
	}

	sigemptyset(&sa.sa_mask);

	sa.sa_flags = 0;
	sa.sa_sigaction = collect_zombie;
	sa.sa_flags = SA_SIGINFO;

	sigaction(SIGCHLD, &sa, NULL);

	if (pipe(writepipe) != 0) {
		print_error_and_exit("APP_TRACKER ==> pipe() failed");
	}
	if (pipe(readpipe) != 0) {
		print_error_and_exit("APP_TRACKER ==> pipe() failed");
	}

	args_dup = calloc(argc, sizeof(char *));
	if (!args_dup) {
		print_error_and_exit("APP_TRACKER ==> calloc() failed");
	}

	for (int i = 1; i < argc; i++) {
		args_dup[i - 1] = calloc(WORDS_SIZE, sizeof(char));
		if (!args_dup[i - 1]) {
			print_error_and_exit("APP_TRACKER ==> calloc() failed");
		}
		strncpy(args_dup[i - 1], args[i], WORDS_SIZE - 1);
	}

	char *bin_file = args_dup[0];

	if ((childpid = fork()) < 0) {
		print_error_and_exit("APP_TRACKER ==> fork() failed");
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
		dup2(CHILD_READ, STDIN_FILENO);
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

		errno = 0;
		if (execvp(bin_file, args_dup) == -1) {
			print_error_and_exit("APP_TRACKER ==> execvp() failed errno: %s", strerror(errno));
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
