#ifndef __STDLIBS__
#define __STDLIBS__

#include <stdlib.h>
#include <stdio.h>

#endif

#define BRANCH 0
#define LEAF 1

struct node{
	struct node *left;
	struct node *right;
	int type;
	char data[32];
};

