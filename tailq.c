
/*
https://man7.org/linux/man-pages/man3/tailq.3.html

FIFO

OUTPUT:
2
1
0

*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>


struct entry {
	int data;
	TAILQ_ENTRY(entry) entries;			/* Tail queue */
};

TAILQ_HEAD(tailhead, entry);

int main(void)
{
	struct entry *n1, *n2, *n3, *np;
	struct tailhead head;				 /* Tail queue head */
	int i;

	TAILQ_INIT(&head);				  /* Initialize the queue */

	n1 = malloc(sizeof(struct entry));	 /* Insert at the head */
	TAILQ_INSERT_HEAD(&head, n1, entries);

	n1 = malloc(sizeof(struct entry));	 /* Insert at the tail */
	TAILQ_INSERT_TAIL(&head, n1, entries);

	n2 = malloc(sizeof(struct entry));	 /* Insert after */
	TAILQ_INSERT_AFTER(&head, n1, n2, entries);

	n3 = malloc(sizeof(struct entry));	 /* Insert before */
	TAILQ_INSERT_BEFORE(n2, n3, entries);

	TAILQ_REMOVE(&head, n2, entries);	  /* Deletion */
	free(n2);
											/* Forward traversal */
	i = 0;
	TAILQ_FOREACH(np, &head, entries)
		np->data = i++;
											/* Reverse traversal */
	TAILQ_FOREACH_REVERSE(np, &head, tailhead, entries)
		printf("%i\n", np->data);
											/* TailQ deletion */
	n1 = TAILQ_FIRST(&head);
	while (n1 != NULL) {
		n2 = TAILQ_NEXT(n1, entries);
		free(n1);
		n1 = n2;
	}
	TAILQ_INIT(&head);

	exit(EXIT_SUCCESS);
}
