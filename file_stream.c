/*
for stdout;
	-> putchar(int c)
	-> puts(char *str)
	-> int getchar(void)
	-> gets(char *str) !!!!!!!!!!!!! do not use this. use fgets instead
	-> printf()
	-> scanf(template, args ...)
*/

#define _GNU_SOURCE //fcloseall
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

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

int main(void)
{
	int i = 0;
	FILE *fs = fopen("streamfile", "w+");

	if (!fs)
		print_error_and_exit("open failed\n");
	
	flockfile(fs);

	fputc((int)'a', fs);
	fputc((int)'b', fs);
	fputc((int)'c', fs);
	fputc((int)'\n', fs);

	fputs("hello world\n", fs);

	fprintf(fs, "%d %s\n", 3, "ppp");

	sync();

	fseek(fs, 0, SEEK_SET);
	int c = 0;
	while ((c = (int)fgetc(fs)) != EOF) {
		printf("%c", (char)c);
	}

	// RETURN
	// abc
	// hello world
	// 3 ppp

	printf("------- seperator -------\n");

	rewind(fs);
	char s[100] = {0};
	while (fgets(s, 100, fs) != NULL) {
		i++;
		printf("asd: %s", (char *)s);

		if (i == 3) {
			long lon = 0;
			char data[20] = {0};

			errno = 0;
			sscanf(s, "%ld %s\n", &lon, data);
			printf("errno: %d\n", errno);

			printf("int: %ld, %s\n", lon, data);
		}
		memset(s, 0, sizeof(s));
	}

	// RETURN:
	// asd: abc
	// asd: hello world
	// asd: 3 ppp
	// errno: 0
	// int: 3, ppp

	printf("------- seperator -------\n");

	rewind(fs);
	char sp[100] = {0};
	fread(sp, sizeof(sp), 1, fs);
	printf("%s", sp);

	// RETURN:
	// abc
	// hello world
	// 3 ppp

	funlockfile(fs);

	fclose(fs);
	fcloseall();

	return 0;
}
