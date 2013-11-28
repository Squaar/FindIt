/*
*  Matt Dumford - mdumford
*  mdumfo2@uic.edu
*/

#ifndef __STDLIBS__
#define __STDLIBS__

#include <stdio.h>
#include <stdlib.h>

#endif

#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

#include "stack.h"
#include "decisionTree.h"

#define ARGLEN (256 * sizeof(char))

void printDir(char *directory);

int main(int argc, char **argv){
	printf("Matt Dumford - mdumford\nmdumfo2@uic.edu\n\n");

	int i;

	int pathsSize = sizeof(char *) * 8;
	char **paths = malloc(pathsSize);
	for(i=0; i<pathsSize; i++)
		paths[i] = malloc(ARGLEN);
	int nPaths = 0;
 
	int expressionsSize = sizeof(char *) * 8;
	char **expressions = malloc(expressionsSize);
	for(i=0; i<expressionsSize; i++)
		expressions[i] = malloc(ARGLEN);
	int nExpressions = 0;

	//split args into paths and expressions
	if(argc > 1){
		int pathsStill = 1;
		for(i=1; i<argc; i++){
			if(pathsStill){
				if(argv[i][0] == '-'){ //if found first expression
					pathsStill = 0;
					if(nExpressions == expressionsSize -1){
						expressionsSize *=2;
						expressions = realloc(expressions, expressionsSize);
					}
					expressions[nExpressions] = argv[i];
					nExpressions++;
				}
				else{ //add path to paths
					if(nPaths == pathsSize -1){
						pathsSize *=2;
						paths = realloc(paths, pathsSize);
					}
					paths[nPaths] = argv[i];
					nPaths++;
				}
			}
			else{ //do expressions
				if(nExpressions == expressionsSize -1){
					expressionsSize *=2;
					expressions = realloc(expressions, expressionsSize);
				}
				expressions[nExpressions] = argv[i];
				nExpressions++;
			}
		}
	}
	if(nPaths == 0){
		printf("adding current directory\n");
		paths[0] = ".";
		nPaths++;
	}

	printf("paths\n");
	for(i=0; i<nPaths; i++){
		printf("\t%s\n", paths[i]);
	}
	printf("expressions\n");
	for(i=0; i<nExpressions; i++){
		printf("\t%s\n", expressions[i]);
	}
	printf("\n");

	for(i=0; i<nPaths; i++){
		printDir(paths[i]);
		printf("\n");
	}

    // //cleanup
    // for(i=0; i<pathsSize; i++){
    // 	printf("free\n"); 
    // 	fflush(stdout);
    // 	free(paths[i]);
    // }
    // printf("poop");
    // free(paths);
    // printf("paths freed");
    // for(i=0; i<expressionsSize; i++)
    // 	free(expressions[i]);
    // free(expressions);

    
    return 0;
}

void printDir(char *directory){
	printf("%s\n", directory);

	DIR *currentDir;
	if((currentDir = opendir(directory)) == NULL){
			perror("Error opening directory");
			exit(-1);
	}

	struct dirent *dir;
	while((dir = readdir(currentDir)) != NULL){
		if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){
			if(dir->d_type == DT_DIR){
				char subDir[ARGLEN];
				strcpy(subDir, directory);
				strcat(subDir, "/");
				strcat(subDir, dir->d_name);
				printDir(subDir);
			}
			else{
				printf("%s/%s\n", directory, dir->d_name);
			}
			//if(dir->d_type == DT_REG)
		}
	}
	closedir(currentDir);
}
