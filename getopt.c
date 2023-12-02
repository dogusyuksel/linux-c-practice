#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <stdbool.h>

#define OK  0
#define NOK 1

#define OPTIONAL_ARGUMENT_IS_PRESENT \
	 ((optarg == NULL && optind < argc && argv[optind][0] != '-') \
	? (bool) (optarg = argv[optind++]) \
	: (optarg != NULL))

static struct option parameters[] = {
	{ "aaa",		required_argument,	0,		'a'		},
	{ "bbb",		no_argument,		0,		'b'		},
	{ "ccc",		optional_argument,	0,		'c'		},
	{ NULL,			0,					0,		 0 		},
};

int main(int argc, char **argv)
{
	 int c, o;
	while ((c = getopt_long(argc, argv, "ba:c::", parameters, &o)) != -1) {
		switch (c) {
			case 'a':
				printf("'a' argument's data is %s\n", optarg);
				break;
			case 'b':
				printf("'b' is the argument\n");
				break;
			case 'c':
				if (OPTIONAL_ARGUMENT_IS_PRESENT) {
					 printf("'c' argument's data is %s\n", optarg);
				} else {
					 printf("'c' is the argument\n");
				}
				break;
			default:
				printf("unknown argument\n");
				return NOK;
		}
	}
}

