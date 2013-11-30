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
void treeDir(char *directory, char *dirName, int depth);
int parseTree(struct node *root, struct dirent *dir);


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
		paths[0] = ".";
		nPaths++;
	}
	if(nExpressions == 0){
		expressions[0] = "-print";
		nExpressions++;
	}

	//=====================DONE SETTING UP ARGS========================

	// struct stringStack stack;
	// stackInit(&stack);
	// for(i=0; i<nExpressions; i++){
	// 	if(strcmp(expressions[i], ")")){
	// 		push(&stack, expressions[i]);
	// 	}
	// 	else{ //if you find a )
	// 		while(stack.height > 0 && strcmp("(", peek(&stack))){

	// 		}
	// 		pop(&stack); //pop the matching (
	// 	}
	// }

	//===================DONE PARSING EXPRESSIONS======================

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
		//printDir(paths[i]);
		treeDir(paths[i], paths[i], 0);
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

void treeDir(char *dirPath, char *dirName,  int depth){
	char dirStr[64] = "";
	int i;
	if(depth > 0){
		for(i=0; i<depth-1; i++){
			strcat(dirStr, "| ");
		}
		strcat(dirStr, "+-");
	}
	strcat(dirStr, dirName);
	//ADD STATISTICS
	printf("%s\n", dirStr);
	
	DIR *currentDir;
	if((currentDir = opendir(dirPath)) == NULL){
		perror("Error opening directory");
		exit(-1);
	}

	struct dirent *dir;
	while((dir = readdir(currentDir)) != NULL){
		if(strcmp(dir->d_name, ".") !=0 && strcmp(dir->d_name, "..") != 0){
			if(dir->d_type == DT_DIR){
				char subDir[ARGLEN];
				strcpy(subDir, dirPath);
				strcat(subDir, "/");
				strcat(subDir, dir->d_name);
				treeDir(subDir, dir->d_name, depth+1);
			}
			else{
				char fileStr[64] = "";
				for(i=0; i<depth; i++){
					strcat(fileStr, "| ");
				}
				strcat(fileStr, "+-");
				strcat(fileStr, dir->d_name);
				printf("%s\n", fileStr);
			}
		}
	}
}

int parseTree(struct node *root, struct dirent *dir){
	if(root->type == BRANCH){
		if(root->operate == AND)
			return (parseTree(root->left, dir) && parseTree(root->right, dir));
		else if(root->operate == OR)
			return (parseTree(root->left, dir) || parseTree(root->right, dir));
		else if(root->operate == NOT)
			return (!parseTree(root->left, dir));
	}
	else{
		//check data to see what about the file to test
	}
	return 0;
}
