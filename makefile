all: findIt

findIt: findIt.c stack.h decisionTree.h
	gcc -Wall findIt.c -o findIt

gdb: findIt.c stack.h decisionTree.h
	gcc -Wall -g findIt.c -o findIt

clean: 
	rm findIt
