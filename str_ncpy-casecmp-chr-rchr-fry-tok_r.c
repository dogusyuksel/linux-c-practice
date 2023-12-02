#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int main(void)
{
/*****************************************************************************************/

	char *s_strncpy_source = "this is strncpy string";
	char s_strncpy[10];
	
	strncpy(s_strncpy, s_strncpy_source, sizeof(s_strncpy));
	printf("s_strncpy lenght is %lu while strlen is %lu, \"%s\"-\"%s\"\n",
		strlen(s_strncpy), strlen(s_strncpy_source), s_strncpy, s_strncpy_source);

	//RETURN: s_strncpy lenght is 10 while strlen is 22, "this is st"-"this is strncpy string"

/*****************************************************************************************/

	//case differences are ignored
	char *s_strcasecmp_source1 = "this is strcasecmp string";
	char *s_strcasecmp_source2 = "This Is strcasecmp STRING";
	
	printf("strcasecmp return is: %d, case diff are ignored\n",
		strcasecmp(s_strcasecmp_source1, s_strcasecmp_source2));

	//RETURN: strcasecmp return is: 0, case diff are ignored

/*****************************************************************************************/

	char *s_strchr = "this is strchr string";
	
	printf("strchr is: \"%s\", strrchr is: \"%s\"\n",
		strchr(s_strchr, (int)'s'), strrchr(s_strchr, (int)'s'));
	
	//RETURN: strchr is: "s is strchr string", strrchr is: "string"

/*****************************************************************************************/

	char s_strfry[22] = "this is strfry string";
	
	printf("strfry is: \"%s\"\n",
		strfry(s_strfry));
	
	//RETURN: strfry is: "issgt isi rsn tthyrfr"

/*****************************************************************************************/

	char s_strtok_r[31] = "this is strtok string1,string2";
	char *token1 = NULL, *restbuff1 = NULL, *str1 = NULL;
	char *token2 = NULL, *restbuff2 = NULL, *str2 = NULL;

	for (str1 = s_strtok_r; ; str1 = NULL) {
		token1 = strtok_r(str1, " ", &restbuff1);
		if (token1 == NULL)
			break;
		printf(" --> %s\n", token1);

		if (strchr(token1, (int)',')) {
			for (str2 = token1; ; str2 = NULL) {
				token2 = strtok_r(str2, ",", &restbuff2);
				if (token2 == NULL)
					break;
				printf("	--> %s\n", token2);
			}
		}
	}

	// RETURN:
	//  --> this
	// --> is
	// --> strtok
	// --> string1,string2
	// 	--> string1
	// 	--> string2

/*****************************************************************************************/

	 exit(EXIT_SUCCESS);
}
