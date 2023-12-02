#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int main(void)
{
/*****************************************************************************************/

	char s_memmove[23] = "this is memmove string";
	char s_memmove_original[23] = "this is memmove string";
	memmove(s_memmove, &s_memmove[2], strlen(s_memmove) - 2);
	printf("after s_memmove \"%s\" became \"%s\"\n", s_memmove_original, s_memmove);

	//RETURN: after s_memmove "this is memmove string" became "is is memmove stringng"

/*****************************************************************************************/

	char mm[10] = {1,2,3,4,5,6,7,8,9,0};
	char needle[2] = {5,6};
	printf("mm: %p, memmem return: %p\n", (int *)mm, (int *)memmem(mm, sizeof(mm), needle, sizeof(needle)));

	//RETURN: mm: 0x7ffec209b916, memmem return: 0x7ffec209b91a

	 exit(EXIT_SUCCESS);
}
