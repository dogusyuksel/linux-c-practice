/*
	 Wen you now delete the first item and then try to find the second, it will not be found, because the
	"other" buckets are only considered when the "optimum" bucket where the hash code is the bucket index is full.

	Besides the non-updating re-additions and the missing deletion option, there are other restrictions to hsearch_r.
	For example, the maximum nuber of entries must be estimated beforehand and cannot be changed later.
	I think hsearch_r is intended as a fast hash table for a limited range of applications.

	source: https://stackoverflow.com/questions/25971505/how-to-delete-element-from-hsearch

	example usage: https://linux.die.net/man/3/hsearch
*/
#include <stdio.h>
#include <stdlib.h>
#include <search.h>

static char *data[] = { "alpha", "bravo", "charlie", "delta",
	  "echo", "foxtrot", "golf", "hotel", "india", "juliet",
	  "kilo", "lima", "mike", "november", "oscar", "papa",
	  "quebec", "romeo", "sierra", "tango", "uniform",
	  "victor", "whisky", "x-ray", "yankee", "zulu"
};

int main(void)
{
	 ENTRY e;
	 ENTRY *ep;

	 hcreate(30);

	 for (int i = 0; i < 24; i++) {
		  e.key = data[i];
		  /* data is just an integer, instead of a
			  pointer to something */
		  e.data = (void *)&i;
		  ep = hsearch(e, ENTER);
		  /* there should be no failures */
		  if (ep == NULL) {
				fprintf(stderr, "entry failed\n");
				exit(EXIT_FAILURE);
		  }
	 }

	 for (int i = 22; i < 26; i++) {
		  /* print two entries from the table, and
			  show that two are not in the table */
		  e.key = data[i];
		  ep = hsearch(e, FIND);
		  printf("%9.9s -> %9.9s:%d\n", e.key,
					ep ? ep->key : "NULL", ep ? *((int *)(ep->data)) : 0);
	 }
	 hdestroy();
	 exit(EXIT_SUCCESS);
}