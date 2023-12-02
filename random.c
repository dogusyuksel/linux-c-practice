#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
	struct timespec ts;

	if (clock_gettime(CLOCK_BOOTTIME, &ts) == -1) {
		perror("clock_gettime");
		exit(1);
	}

	srand((unsigned int)ts.tv_nsec);
	unsigned int randnum = rand();

	printf("random between 100 and 10000 is %d\n", (unsigned int)(100 + (randnum % 10000)));

	char buffer[100] = {0};

	/*Most applications should use getentropy. The getrandom function is
	intended for lowlevel applications which need additional control over blocking behavior.*/
	memset(buffer, 0, sizeof(buffer));
	getentropy(buffer, sizeof(buffer));

	unsigned int i = 0;
	for(i = 0; i < (unsigned int)sizeof(buffer); i++) {
		printf("%d ", buffer[i]);
	}
	printf("\n");

	//OUTPUT:
	// random between 100 and 10000 is 7225
	// -53 -42 -74 67 65 -124 18 -27 -23 -18 -6 -100 -114 -8 125 -57 -32 -38 49 121 -108 
	// 54 -42 -12 -69 120 -95 59 37 80 27 -77 -103 -119 74 113 -108 -128 -4 58 -75 47 55 
	// 25 -50 95 -6 102 17 76 -15 -113 96 111 -99 -123 110 -110 -118 56 -52 113 -84 27 -2 
	// 108 84 -17 -109 -123 74 -5 -25 0 55 34 113 -16 52 -16 44 122 -72 123 -47 -81 40 -125 
	// 4 21 47 108 18 -46 25 126 98 113 -71 -87

	return 0;
}
