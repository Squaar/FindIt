#ifndef __STDLIBS__
#define __STDLIBS__

#include <stdlib.h>
#include <stdio.h>

#endif

#define BRANCH 0
#define LEAF 1
#define AND 0
#define OR 1
#define NOT 2

struct node{
	struct node *left;
	struct node *right;
	int type;
	int operate;
	char *expression;
	char *option;
};

