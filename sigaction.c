#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <err.h>
#include <execinfo.h>
#include <string.h>

#define MAX_STACK_FRAMES 64

static char const *icky_global_program_name;
static void *stack_traces[MAX_STACK_FRAMES];
static char const *icky_global_program_name;

int  divide_by_zero();
void cause_segfault();
void stack_overflow();
void infinite_loop();
void illegal_instruction();
void cause_calamity();

/* Resolve symbol name and source location given the path to the executable 
and an address */
int addr2line(char const *const program_name, char const *addr)
{
	char addr2line_cmd[512] = {0};

	sprintf(addr2line_cmd,"addr2line -e %.256s -f %s", program_name, addr);

	printf("\t\n");

	return system(addr2line_cmd);
}

int getaddr(const char *line, char *address, int size)
{
	char *addr_beg = NULL;
	char *addr_end = NULL;

	if (!line || !sizeof(line)) {
		return 0;
	}

	addr_beg = strrchr(line, (int)'+');
	addr_beg++; //skip +
	addr_end = strrchr(line, (int)')');

	if ((addr_end - addr_beg) > size) {
		return 0;
	}

	memcpy(address, addr_beg, (addr_end - addr_beg));
	address[(addr_end - addr_beg)] = '\0';

	return 1;
}

void posix_print_stack_trace()
{
	int i = 0, f = 0, trace_size = 0;
	char **messages = (char **)NULL;
	char addr[32];

	trace_size = backtrace(stack_traces, MAX_STACK_FRAMES);
	messages = backtrace_symbols(stack_traces, trace_size);

	/* skip the first couple stack frames (as they are this function and
	our handler) and also skip the last frame as it's (always?) junk. */
	for (i = 0; i < trace_size; ++i) {
		printf("%s\n", messages[i]);
		if (strstr(messages[i], icky_global_program_name)) {
			if(!(getaddr(messages[i], addr, 32) && !addr2line(icky_global_program_name, addr))) {
				printf("show partially: % 2d: %s\n", ++f, messages[i]);
			}
		}
	}

	if (messages) {
		free(messages);
	}
}

void posix_signal_handler(int sig, siginfo_t *siginfo, void *context)
{
	(void)context;
	switch(sig) {
		case SIGSEGV:
			fputs("Caught SIGSEGV: Segmentation Fault\n", stderr);
			break;
		case SIGINT:
			fputs("Caught SIGINT: Interactive attention signal, (usually ctrl+c)\n", stderr);
			break;
		case SIGFPE:
			switch(siginfo->si_code)
			{
				case FPE_INTDIV:
					fputs("Caught SIGFPE: (integer divide by zero)\n", stderr);
					break;
				case FPE_INTOVF:
					fputs("Caught SIGFPE: (integer overflow)\n", stderr);
					break;
				case FPE_FLTDIV:
					fputs("Caught SIGFPE: (floating-point divide by zero)\n", stderr);
					break;
				case FPE_FLTOVF:
					fputs("Caught SIGFPE: (floating-point overflow)\n", stderr);
					break;
				case FPE_FLTUND:
					fputs("Caught SIGFPE: (floating-point underflow)\n", stderr);
					break;
				case FPE_FLTRES:
					fputs("Caught SIGFPE: (floating-point inexact result)\n", stderr);
					break;
				case FPE_FLTINV:
					fputs("Caught SIGFPE: (floating-point invalid operation)\n", stderr);
					break;
				case FPE_FLTSUB:
					fputs("Caught SIGFPE: (subscript out of range)\n", stderr);
					break;
				default:
					fputs("Caught SIGFPE: Arithmetic Exception\n", stderr);
					break;
			}
			break;
		case SIGILL:
			switch(siginfo->si_code)
			{
				case ILL_ILLOPC:
					fputs("Caught SIGILL: (illegal opcode)\n", stderr);
					break;
				case ILL_ILLOPN:
					fputs("Caught SIGILL: (illegal operand)\n", stderr);
					break;
				case ILL_ILLADR:
					fputs("Caught SIGILL: (illegal addressing mode)\n", stderr);
					break;
				case ILL_ILLTRP:
					fputs("Caught SIGILL: (illegal trap)\n", stderr);
					break;
				case ILL_PRVOPC:
					fputs("Caught SIGILL: (privileged opcode)\n", stderr);
					break;
				case ILL_PRVREG:
					fputs("Caught SIGILL: (privileged register)\n", stderr);
					break;
				case ILL_COPROC:
					fputs("Caught SIGILL: (coprocessor error)\n", stderr);
					break;
				case ILL_BADSTK:
					fputs("Caught SIGILL: (internal stack error)\n", stderr);
					break;
				default:
					fputs("Caught SIGILL: Illegal Instruction\n", stderr);
					break;
			}
			break;
		case SIGTERM:
			fputs("Caught SIGTERM: a termination request was sent to the program\n", stderr);
			break;
		case SIGABRT:
			fputs("Caught SIGABRT: usually caused by an abort() or assert()\n", stderr);
			break;
		case SIGUSR1:
			fputs("Caught SIGUSR1\n", stderr);
			break;
		default:
			break;
	}
	posix_print_stack_trace();
	exit(1);
}

void set_signal_handler()
{
	/* setup alternate stack */
	{
		stack_t ss = {};

		ss.ss_sp = malloc(SIGSTKSZ);
		if (ss.ss_sp == NULL) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		ss.ss_size = SIGSTKSZ;
		ss.ss_flags = 0;

		/*sigaltstack() allows a thread to define a new alternate signal
		stack and/or retrieve the state of an existing alternate signal
		stack.  An alternate signal stack is used during the execution of
		a signal handler if the establishment of that handler (see
		sigaction(2)) requested it.*/
		if (sigaltstack(&ss, NULL) != 0) {
			perror("sigaltstack");
			exit(EXIT_FAILURE);
		}
	}

	/* register our signal handlers */
	{
		struct sigaction sig_action = {};
		sigset_t sigset;

		sig_action.sa_sigaction = posix_signal_handler;
		sigemptyset(&sig_action.sa_mask);
		sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;

		if (sigaction(SIGSEGV, &sig_action, NULL) != 0) {perror("sigaction"); exit(EXIT_FAILURE);}
		if (sigaction(SIGFPE,  &sig_action, NULL) != 0) {perror("sigaction"); exit(EXIT_FAILURE);}
		if (sigaction(SIGINT,  &sig_action, NULL) != 0) {perror("sigaction"); exit(EXIT_FAILURE);}
		if (sigaction(SIGILL,  &sig_action, NULL) != 0) {perror("sigaction"); exit(EXIT_FAILURE);}
		if (sigaction(SIGTERM, &sig_action, NULL) != 0) {perror("sigaction"); exit(EXIT_FAILURE);}
		if (sigaction(SIGABRT, &sig_action, NULL) != 0) {perror("sigaction"); exit(EXIT_FAILURE);}
		if (sigaction(SIGUSR1, &sig_action, NULL) != 0) {perror("sigaction"); exit(EXIT_FAILURE);}

		sigemptyset(&sigset);
		sigaddset(&sigset, SIGUSR1);
		if(sigprocmask(SIG_BLOCK, &sigset, NULL) == 0) {
			printf("Blocking SISGUSR1...\n");
		}

		kill(getpid(), SIGUSR1);

		sleep(2);

		sigemptyset(&sigset);
		sigpending(&sigset);
		if (sigismember(&sigset, SIGUSR1) == 1) {
			printf("The signal is blocked %d\n", __LINE__);
		}

		//after unblocking, handler got the signal which was pending
		// sigemptyset(&sigset);
		// sigaddset(&sigset, SIGUSR1);
		// if(sigprocmask(SIG_UNBLOCK, &sigset, NULL) == 0) {
		// 	printf("Unblocking SISGUSR1...\n");
		// }
	}
}

int main(int argc, char * argv[])
{
	(void)argc;

	/* store off program path so we can use it later */
	icky_global_program_name = argv[0];

	set_signal_handler();

	cause_calamity();

	puts("OMG! Nothing bad happend!");

}

void cause_calamity()
{
	/* uncomment one of the following error conditions to cause a calamity of 
	your choosing! */

	//(void)divide_by_zero();
	//cause_segfault();
	assert(false);
	//infinite_loop();
	//illegal_instruction();
	//stack_overflow();
}



int divide_by_zero()
{
	int a = 1;
	int b = 0; 
	return a / b;
}

void cause_segfault()
{
	int * p = (int*)0x12345678;
	*p = 0;
}

void stack_overflow()
{
	int foo[1000];
	(void)foo;
	stack_overflow();
}

/* break out with ctrl+c to test SIGINT handling */
void infinite_loop()
{
	while(1) {};
}

void illegal_instruction()
{
	/* I couldn't find an easy way to cause this one, so I'm cheating */
	raise(SIGILL);
}
