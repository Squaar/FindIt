#ifndef __STDLIBS__
#define __STDLIBS__

#include <stdio.h>
#include <stdlib.h>

#endif

struct stringStack {
	char **stack;
	int height;
	int size;
};

const int STACK_ELEMENT_SIZE = 32;

void stackInit(struct stringStack *stack){
	stack->height = 0;
	stack->size = 8;
	stack->stack = malloc(sizeof(char *) * stack->size);
	int i;
	for(i=0; i<stack->size; i++){
		stack->stack[i] = malloc(sizeof(char) * STACK_ELEMENT_SIZE);
	}
}

void push(struct stringStack *stack, char *element){
	if(stack->height == stack->size-1){
		stack->size *=2;
		stack->stack = realloc(stack->stack, stack->size * sizeof(char *));
	}
	stack->stack[stack->height] = element;
	stack->height++;
}

char *pop(struct stringStack *stack){
	stack->height--;
	char *ans = stack->stack[stack->height];
	stack->stack[stack->height] = NULL;
	return ans;
}

void freeStack(struct stringStack *stack){
	int i;
	for(i=0; i<stack->size; i++){
		free(stack->stack[i]);
	}
	free(stack->stack);
}
