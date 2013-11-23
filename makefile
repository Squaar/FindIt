all: findIt

findIt: findIt.c
	gcc -Wall findIt.c -o findIt

gdb: findIt.c
	gcc -Wall -g findIt.c -o findIt

clean: 
	rm findIt