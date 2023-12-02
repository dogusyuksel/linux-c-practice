	// OUTPUT:
	// **********************************************************
	// CLOCK_REALTIME : 1685604063.267 (19509 days +  7h 21m  3s)
	// 	1 5 123 - 10:21:3.267
	// CLOCK_TAI	 : 1685604063.267 (19509 days +  7h 21m  3s)
	// CLOCK_MONOTONIC:	162014.364 (1 days + 21h  0m 14s)
	// CLOCK_BOOTTIME :	257861.041 (2 days + 23h 37m 41s)
	// CLOCK_PROCESS_CPUTIME_ID:		0.005 ( 0h  0m  0s)
	// CLOCK_THREAD_CPUTIME_ID:		0.005 ( 0h  0m  0s)
	// **********************************************************
	// CLOCK_REALTIME : 1685604064.267 (19509 days +  7h 21m  4s)
	// 	1 5 123 - 10:21:4.267
	// CLOCK_TAI	 : 1685604064.268 (19509 days +  7h 21m  4s)
	// CLOCK_MONOTONIC:	162015.364 (1 days + 21h  0m 15s)
	// CLOCK_BOOTTIME :	257862.041 (2 days + 23h 37m 42s)
	// CLOCK_PROCESS_CPUTIME_ID:		0.005 ( 0h  0m  0s)
	// CLOCK_THREAD_CPUTIME_ID:		0.005 ( 0h  0m  0s)
	// **********************************************************
	// CLOCK_REALTIME : 1685604065.268 (19509 days +  7h 21m  5s)
	// 	1 5 123 - 10:21:5.268
	// CLOCK_TAI	 : 1685604065.268 (19509 days +  7h 21m  5s)
	// CLOCK_MONOTONIC:	162016.364 (1 days + 21h  0m 16s)
	// CLOCK_BOOTTIME :	257863.041 (2 days + 23h 37m 43s)
	// CLOCK_PROCESS_CPUTIME_ID:		0.005 ( 0h  0m  0s)
	// CLOCK_THREAD_CPUTIME_ID:		0.005 ( 0h  0m  0s)
	// **********************************************************


#define _XOPEN_SOURCE 600
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define SECS_IN_DAY (24 * 60 * 60)

static void
display_clock(clockid_t clock, const char *name)
{
	struct timespec ts;

	if (clock_gettime(clock, &ts) == -1) {
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}

	printf("%-15s: %10jd.%03ld (", name,
			(intmax_t) ts.tv_sec, ts.tv_nsec / 1000000);

	long days = ts.tv_sec / SECS_IN_DAY;
	if (days > 0) {
		printf("%ld days + ", days);
	}

	printf("%2dh %2dm %2ds",
			(int) (ts.tv_sec % SECS_IN_DAY) / 3600,
			(int) (ts.tv_sec % 3600) / 60,
			(int) ts.tv_sec % 60);
	printf(")\n");

	if (clock == CLOCK_REALTIME) {
		//show brokentime
		struct tm *brokentime = localtime(&(ts.tv_sec));
		printf("	 %d %d %d - %d:%d:%d.%ld\n", brokentime->tm_mday, brokentime->tm_mon, brokentime->tm_year, 
			brokentime->tm_hour, brokentime->tm_min, brokentime->tm_sec, (ts.tv_nsec / 1000000));
	}
}

int main(void)
{

	while(1) {
		display_clock(CLOCK_REALTIME, "CLOCK_REALTIME");
		//this is realtime clock
#ifdef CLOCK_TAI
		display_clock(CLOCK_TAI, "CLOCK_TAI");
		// A nonsettable system-wide clock derived from wall-clock
		// time but ignoring leap seconds.  This clock does not
		// experience discontinuities and backwards jumps caused by
		// NTP inserting leap seconds as CLOCK_REALTIME does.
#endif
		display_clock(CLOCK_MONOTONIC, "CLOCK_MONOTONIC");
		// Counts from system boot but it does not count time that the system is suspended
#ifdef CLOCK_BOOTTIME
		display_clock(CLOCK_BOOTTIME, "CLOCK_BOOTTIME");
		// A nonsettable system-wide clock that is identical to
		// CLOCK_MONOTONIC, except that it also includes any time
		// that the system is suspende
#endif
#ifdef CLOCK_PROCESS_CPUTIME_ID
		display_clock(CLOCK_PROCESS_CPUTIME_ID, "CLOCK_PROCESS_CPUTIME_ID");
		// This is a clock that measures CPU time consumed by this
		// process (i.e., CPU time consumed by all thread_mutex in the
		// process)
#endif
#ifdef CLOCK_THREAD_CPUTIME_ID
		display_clock(CLOCK_THREAD_CPUTIME_ID, "CLOCK_THREAD_CPUTIME_ID");
		// This is a clock that measures CPU time consumed by this thread
#endif
		printf("**********************************************************\n");
		sleep(1);
	}

	exit(EXIT_SUCCESS);
}
