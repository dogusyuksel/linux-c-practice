/*
* Root: A root is a node without a parent.
* Siblings: Siblings mean that nodes which have the same parent node.
* Internal Node: Internal Node means that a node which has at least a single child.
* External Node: External Node means that a node which has no children. It is also
known as leaf.
* Ancestors: Ancestors include the parent, grandparent and so on of a node.
* Descendants: Descendants are the opposite of ancestors, It includes the child,
grandchild and so on of a node.
* Edge: An edge means a connection between one node to another node.
* Path: Path is a combination of nodes and edges connected with each other.
* Depth: You can calculate depth by the number of edges from node to the root of the
tree.
* Height: Height is the maximum depth of a node.
* Level: Level of a node is equal to depth of the node + 1.

* Proper Binary Tree (Full Binary Tree)
In a proper binary tree, every internal node has exactly wo children. A binary tree that
is not proper is improper

* Balanced Binary Tree
A balanced binary tree, also referred to as a height-balanced binary tree, is defined
as a binary tree in which the height of the left and right (node to leaf, depth is node to
root) subtree of any node differ by not more than 1. To do that, all nodes' lef and right
subtrees also should be balanced

* Full Binary Tree
every parent node / internal node has either two or no children (it means, if all leafes
have siblings)18

* Perfect Binary Tree
all internal nodes have two children and all leafes are at same level

* Complete Binary Tree
every level must be completely filled. all the leaf elements must lean towards the left.
the last leaf element might not have a right sibling.

* Degenerate or Pathological Tree
having a single children left or right

* Skewer Binary Tree
similar to degenerate but have children only one side
*/

#define _GNU_SOURCE /* Expose declaration of tdestroy() */
#include <search.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static void *root = NULL;

static int compare(const void *pa, const void *pb)
{
	return (*(int *)pa - *(int *)pb);
}

static void action(const void *nodep, VISIT which, int depth)
{
	/*preorder, postorder, and endorder are known as preorder, inorder, and postorder:
	before visiting the children, after the first and before the second, and after visiting the
	children. Thus, the choice of name postorder is rather confusing*/
	int *datap;

	switch (which) {
		case preorder:
		break;
		case postorder:
			datap = *(int **) nodep;
			printf("%6d\tdepth: %d\n", *datap, depth);
		break;
		case endorder:
		break;
		case leaf:
			datap = *(int **) nodep;
			printf("%6d\tdepth: %d\n", *datap, depth);
		break;
	}
}

static void freenode(void *nodep)
{
	//since our nodes are just integer pointer
	//simple free is enough
	if (nodep) {
		free(nodep);
	}
}

int main(void)
{
	int i = 0;
	int **val = NULL;
	int dummy[11] = {179, 102, 220, 124, 214, 40, 16, 12, 70, 161, 35};

	for (i = 0; i < 11; i++) {
		int *ptr = (int *)malloc(sizeof(int));
		if (!ptr) {
			perror("malloc failed\n");
			goto out;
		}

		*ptr = dummy[i];

		//If the tree does not contain a matching entry the key value will be added to the tree
		val = tsearch(ptr, &root, compare);

		if (*val != ptr) {
			goto out;
		}
	}

	twalk(root, action);

	if (NULL != tfind(&dummy[10], &root, compare)) {
		printf("%d is found\n", dummy[10]);

		if (NULL != tdelete(&dummy[10], &root, compare)) {
			printf("%d is deleted successfully\n", dummy[10]);

			twalk(root, action);
		}
	}

out:
	tdestroy(root, freenode);

	exit(EXIT_SUCCESS);
}

/*OUTPUT:
	 12	depth: 2
	 16	depth: 1
	 35	depth: 3
	 40	depth: 2
	 70	depth: 3
	102	depth: 0
	124	depth: 2
	161	depth: 3
	179	depth: 1
	214	depth: 3
	220	depth: 2
35 is found
35 is deleted successfully
	 12	depth: 2
	 16	depth: 1
	 40	depth: 2
	 70	depth: 3
	102	depth: 0
	124	depth: 2
	161	depth: 3
	179	depth: 1
	214	depth: 3
	220	depth: 2*/
