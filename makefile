all: findIt

findIt: findIt.c
	gcc -Wall findIt.c -o findIt

clean: 
	rm findIt