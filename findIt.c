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
#include <ftw.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "stack.h"
#include "decisionTree.h"

#define ARGLEN (256 * sizeof(char))

#define BOOL int
#define TRUE 1
#define FALSE 0

struct dirStats{
	int files;
	int dirs;
	int size;
};

void printDir(char *directory, struct node *expressionRoot);
void treeDir(char *directory, char *dirName, int depth);
BOOL parseTree(struct node *root, struct dirent *dir);
struct dirStats getDirStats(char *dirPath);
int sizeSum(const char *fpath, const struct stat *sb, int typeflag);
void summaryTable(char *dirPath);

int sum = 0;


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

	struct node root;	//TEMPORARY FOR TESTING
	root.type = LEAF;

	//===================DONE PARSING EXPRESSIONS======================



	// printf("paths\n");
	// for(i=0; i<nPaths; i++){
	// 	printf("\t%s\n", paths[i]);
	// }
	// printf("expressions\n");
	// for(i=0; i<nExpressions; i++){
	// 	printf("\t%s\n", expressions[i]);
	// }
	// printf("\n");

	for(i=0; i<nPaths; i++){
		if(!strcmp(expressions[0], "-treedir"))
			treeDir(paths[i], paths[i], 0);
		else if(!strcmp(expressions[0], "-summarize"))
			summaryTable(paths[i]);
		else
			printDir(paths[i], &root);
		printf("\n");
	}

    // //cleanup
    // for(i=0; i<pathsSize; i++){
    // 	printf("free\n"); 
    // 	fflush(stdout);
    // 	free(paths[i]);
    // }
    // free(paths);
    // printf("paths freed");
    // for(i=0; i<expressionsSize; i++)
    // 	free(expressions[i]);
    // free(expressions);
    
    return 0;
}

void summaryTable(char *dirPath){
	int files = 0;
	int dirs = 0;
	int charDevs = 0;
	int blockDevs = 0;
	int fifos = 0;
	int links = 0;
	int sockets = 0;
	int unknowns = 0;

	int fileSize = 0;
	int dirSize = 0;
	int charDevSize = 0;
	int blockDevSize = 0;
	int fifoSize = 0;
	int linkSize = 0;
	int socketSize = 0;
	int unknownSize = 0;

	DIR *dirP;
	if((dirP = opendir(dirPath)) == NULL){
			perror("Error opening directory");
			exit(-1);
	}

	struct dirent *dir;
	while((dir = readdir(dirP)) != NULL){
		if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){
			char subDir[ARGLEN];
			strcpy(subDir, dirPath);
			strcat(subDir, "/");
			strcat(subDir, dir->d_name);
			
			struct stat stats;
			if(lstat(subDir, &stats)){
				perror("Error getting stats");
				exit(-1);
			}

			switch(stats.st_mode & S_IFMT){
				case S_IFBLK:
					blockDevs++;
					blockDevSize += stats.st_size;
					break;
			    case S_IFCHR:
			    	charDevs++;
			    	charDevSize += stats.st_size;
			    	break;
			    case S_IFDIR:
			    	dirs++;
			    	dirSize += stats.st_size;
			    	break;
			    case S_IFIFO:
			    	fifos++;
			    	fifoSize += stats.st_size;
			    	break;
			    case S_IFLNK:
			    	links++;
			    	linkSize += stats.st_size;
			    	break;
			    case S_IFREG:
			    	files++;
			    	fileSize += stats.st_size;
			    	break;
			    case S_IFSOCK:
			    	sockets++;
			    	socketSize += stats.st_size;
			    	break;
			    default:
			    	unknowns++;
			    	unknownSize += stats.st_size;
			    	break;
			}
		}
	}
	closedir(dirP);

	if(files > 0)
		printf("Regular Files: %i found, %i bytes\n", files, fileSize);
	if(dirs > 0)
		printf("Directories: %i found, %i bytes\n", dirs, dirSize);
	if(charDevs > 0)
		printf("Character Devices: %i found, %i bytes\n", charDevs, charDevSize);
	if(blockDevs > 0)
		printf("Block Devices: %i found, %i bytes\n", blockDevs, blockDevSize);
	if(fifos > 0)
		printf("Fifo Pipes: %i found, %i bytes\n", fifos, fifoSize);
	if(links > 0)
		printf("Links: %i found, %i bytes\n", links, linkSize);
	if(sockets > 0)
		printf("Sockets: %i found, %i bytes\n", sockets, socketSize);
	if(unknowns > 0)
		printf("Unknown Files: %i found, %i bytes\n", unknowns, unknownSize);
}

void printDir(char *directory, struct node *expressionRoot){
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
				printDir(subDir, expressionRoot);
			}
			else{
				if(parseTree(expressionRoot, dir))
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

	struct dirStats stats = getDirStats(dirPath);

	printf("%s(%i dirs, %i files, %i bytes)\n", dirStr, stats.dirs, stats.files, stats.size);
	
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
		}
	}
	closedir(currentDir);
}

struct dirStats getDirStats(char *dirPath){
	struct dirStats ans;
	ans.dirs = ans.files = ans.size = 0;

	DIR *dirP;
	if((dirP = opendir(dirPath)) == NULL){
		perror("Error opening directory");
		exit(-1);
	}

	struct dirent *dir;
	while((dir = readdir(dirP)) != NULL){
		if(strcmp(dir->d_name, ".") !=0 && strcmp(dir->d_name, "..") != 0){
			if(dir->d_type == DT_DIR)
				ans.dirs++;
			else
				ans.files++;
		}
	}

	if(ftw(dirPath, &sizeSum, 1)){
		perror("Error getting size of directory");
		exit(-1);
	}
	ans.size = sum;
	sum = 0;
	return ans;
}

int sizeSum(const char *fpath, const struct stat *sb, int typeflag){
	sum += sb->st_size;
	return 0;
}

BOOL parseTree(struct node *root, struct dirent *dir){
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
	return TRUE;
}
