/*
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
*/

#define _GNU_SOURCE //asprintf
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void custom_fprint(FILE *out_stream, const char *format, ...)
{
	va_list mlist;
	char *buffer = NULL;

	va_start(mlist, format);
	vasprintf(&buffer, format, mlist);
	va_end(mlist);

	if (!buffer)
		exit(1);

	fprintf(out_stream, "%s\n", buffer);
	free(buffer);
}

int main(void)
{
	 custom_fprint(stderr, "printed error");
	 custom_fprint(stdout, "printed log");

	 exit(EXIT_SUCCESS);
}