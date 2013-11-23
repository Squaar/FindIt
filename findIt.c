/*
*  Matt Dumford - mdumford
*  mdumfo2@uic.edu
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

int main(int argc, char **argv){
	printf("Matt Dumford - mdumford\nmdumfo2@uic.edu\n\n");

    DIR *currentDir;
    if((currentDir = opendir(".")) == NULL){
    	perror("Error opening directory");
    	exit(-1);
    }

    struct dirent *dir;
    while((dir = readdir(currentDir)) != NULL){
		printf("%s\n", dir->d_name);
    }

    printf("\ndone\n");

    return 0;
}