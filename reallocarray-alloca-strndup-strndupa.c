#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int main(void)
{
	//it should be used instead of realloc because newsize prm of it lead us to set it wrong
	int *ptr_reallocarray = (int *)reallocarray(NULL, 20, sizeof(int));
	if (!ptr_reallocarray) {
		perror("reallocarray failed\n");
		exit(EXIT_FAILURE);
	}
	
	printf("reallocarray allocates 20 integer\n");
	free(ptr_reallocarray);

	//RETURN: reallocarray allocates 20 integer

/*****************************************************************************************/

	//allocates memory from STACK. No need explicit FREE operation
	int *ptr_alloca = (int *)alloca(20 * sizeof(int));
	if (!ptr_alloca) {
		perror("alloca failed\n");
	}

	printf("alloca allocates 20 integer\n");
	//no need to free it

	//RETURN: alloca allocates 20 integer

/*****************************************************************************************/

	char *s_strndup_source = "this is strndup string";
	char *s_strndup = strndup(s_strndup_source, 10);
	if (!s_strndup) {
		perror("strndup failed\n");
	}

	printf("s_strndup lenght is %lu while strlen is %lu, \"%s\"-\"%s\"\n",
		strlen(s_strndup), strlen(s_strndup_source), s_strndup, s_strndup_source);

	free(s_strndup);

	//RETURN: s_strndup lenght is 10 while strlen is 22, "this is st"-"this is strndup string"

/*****************************************************************************************/

	//uses alloca instead of alloc-malloc
	char *s_strndupa_source = "this is strndupa string";
	char *s_strndupa = strndupa(s_strndupa_source, 10);
	if (!s_strndupa) {
		perror("strndup failed\n");
	}

	printf("s_strndupa lenght is %lu while strlen is %lu, \"%s\"-\"%s\"\n",
		strlen(s_strndupa), strlen(s_strndupa_source), s_strndupa, s_strndupa_source);

	//no need to free it

	//RETURN: s_strndupa lenght is 10 while strlen is 23, "this is st"-"this is strndupa string"

/*****************************************************************************************/

	exit(EXIT_SUCCESS);;
}
