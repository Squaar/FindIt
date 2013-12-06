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

//struct used for holding various stats of a directory
struct dirStats{
	int files;
	int dirs;
	int size;
};

//functions
void printDir(char *directory, struct node *expressionRoot, BOOL dirs);
void treeDir(char *directory, char *dirName, int depth);
BOOL parseTree(struct node *root, char *filePath);
struct dirStats getDirStats(char *dirPath);
int sizeSum(const char *fpath, const struct stat *sb, int typeflag);
void summaryTable(char *dirPath);
void largest(char *dirPath, char *type);
void smallest(char *dirPath, char *type);
int largestFile(const char *fpath, const struct stat *sb, int typeflag);
int largestDir(const char *fpath, const struct stat *sb, int typeflag);
int largestTree(const char *fpath, const struct stat *sb, int typeflag);
int smallestFile(const char *fpath, const struct stat *sb, int typeflag);
int smallestDir(const char *fpath, const struct stat *sb, int typeflag);
int smallestTree(const char *fpath, const struct stat *sb, int typeflag);
BOOL hasAccess(char *filePath, char *type);
struct node *makeTree(char **expressions, int nExpressions);
void printTree(struct node *root, int depth);
BOOL size(char *filePath, char *option);
BOOL sparse(char *filePath);
void help();

//globals used for ftw helpers
int sum = 0;
int aInt = -1;
char *aString;

int main(int argc, char **argv){
	printf("Matt Dumford - mdumford\nmdumfo2@uic.edu\n\n");

	int i;
	
	//parse paths and expressions into seperate arrays
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

	//make tree from aray of expressions
	struct node *tree;
	tree = makeTree(expressions, nExpressions);

	aString = malloc(0);

	//parse input
	for(i=0; i<nPaths; i++){
		if(!strcmp(expressions[0], "-treedir"))
			treeDir(paths[i], paths[i], 0);
		else if(!strcmp(expressions[0], "-summarize"))
			summaryTable(paths[i]);
		else if(!strcmp(expressions[0], "-largest")){
			if(nExpressions >= 2){
				largest(paths[i], expressions[1]);
			}
			else
				printf("Not enough Expressions.");
		}
		else if(!strcmp(expressions[0], "-smallest")){
			if(nExpressions >= 2){
				smallest(paths[i], expressions[1]);
			}
			else
				printf("Not enough Expressions.");
		}
		else if(!strcmp(expressions[0], "-help"))
				help();
		else if(nExpressions == 1 && !strcmp(expressions[0], "-print"))
			printDir(paths[i], tree, TRUE);
		else
			printDir(paths[i], tree, FALSE);
		printf("\n");
	}

    return 0;
}

void help(){
	int c;
	FILE *file = fopen("readme.txt", "r");
	if(file){
		while((c = getc(file)) != EOF)
			putchar(c);
		fclose(file);
	}
	else{
		printf("Error opening readme.txt\n");
		exit(-1);
	}
	exit(0);
}

//determine if file has more, less, or exactly a certain number of bytes
BOOL size(char *filePath, char *option){
	struct stat stats;
	if(lstat(filePath, &stats)){
		perror("Error getting stats");
		exit(-1);
	}

	int size = stats.st_size;
	long test;
	if(option[0] == '-'){
		test = strtol(option+1, 0, 10);
		if(size < test)
			return TRUE;
	}
	else if(option[0] == '+'){
		test = strtol(option+1, 0, 10);
		if(size > test)
			return TRUE;
	}
	else{
		test = strtol(option, 0, 10);
		if(size == test)
			return TRUE;
	}

	return FALSE;
}

//determine if a file is sparse
BOOL sparse(char *filePath){
	struct stat stats;
	if(lstat(filePath, &stats)){
		perror("Error getting stats");
		exit(-1);
	}

	if(stats.st_blocks * 512 < stats.st_size)
		return TRUE;
	return FALSE;
}

//determine if user has access to a file
BOOL hasAccess(char *filePath, char *type){
	int mask = 0;
	if(!strcmp(type, "R") || !strcmp(type, "r"))
		mask = R_OK;
	else if(!strcmp(type, "W") || !strcmp(type, "w"))
		mask = W_OK;
	else if(!strcmp(type, "X") || !strcmp(type, "x"))
		mask = X_OK;
	else if(!strcmp(type, "A") || !strcmp(type, "a"))
		mask = R_OK | W_OK | X_OK;
	else{
		printf("I don't know what %s is (hasAccess)\n", type);
		exit(-1);
	}

	return !access(filePath, mask);
}

//find largest file/directory/tree
void largest(char *dirPath, char *type){
	//if user wants largest file
	if(!strcmp(type, "-file") || !strcmp(type, "file")){
		if(ftw(dirPath, &largestFile, 1)){
			perror("Error finding largest file");
			exit(-1);
		}
		printf("The largest File found is: %s (%i bytes)\n", aString, aInt);
	}
	//if user wants largest directory
	else if(!strcmp(type, "-dir") || !strcmp(type, "dir")){
		if(ftw(dirPath, &largestDir, 1)){
			perror("Error finding largest file");
			exit(-1);
		}
		printf("The largest directory found is: %s (%i bytes)\n", aString, aInt);
	}
	//if user wants largest tree
	else if(!strcmp(type, "-tree") || !strcmp(type, "tree")){
		if(ftw(dirPath, &largestTree, 1)){
			perror("Error finding largest file");
			exit(-1);
		}
		printf("The largest tree found is: %s (%i bytes)\n", aString, aInt);
	}
	else
		printf("I don't know what a %s is.", type);

	aInt = -1;
	aString = "";
}

//find smallest file/directory/tree
void smallest(char *dirPath, char *type){
	//if user wants smallest file
	if(!strcmp(type, "-file") || !strcmp(type, "file")){
		if(ftw(dirPath, &smallestFile, 1)){
			perror("Error finding smallest file");
			exit(-1);
		}
		printf("The smallest File found is: %s (%i bytes)\n", aString, aInt);
	}
	//if user wants smallest directory
	else if(!strcmp(type, "-dir") || !strcmp(type, "dir")){
		if(ftw(dirPath, &smallestDir, 1)){
			perror("Error finding smallest file");
			exit(-1);
		}
		printf("The smallest directory found is: %s (%i bytes)\n", aString, aInt);
	}
	//if user wants smallest tree
	else if(!strcmp(type, "-tree") || !strcmp(type, "tree")){
		if(ftw(dirPath, &smallestTree, 1)){
			perror("Error finding smallest file");
			exit(-1);
		}
		printf("The smallest tree found is: %s (%i bytes)\n", aString, aInt);
	}
	else
		printf("I don't know what a %s is.", type);

	aInt = -1;
	aString = "";
}

//helper function for finding largest ile
int largestFile(const char *fpath, const struct stat *sb, int typeflag){
	if(typeflag == FTW_F){
		if(sb->st_size >= aInt){
			aInt = sb->st_size;
			aString = realloc(aString, strlen(fpath) +1);
			aString[strlen(fpath)] = '\0';
			aString = strncpy(aString, fpath, strlen(fpath));
		}
	}
	return 0;
}

//helper function for finding smallest file
int smallestFile(const char *fpath, const struct stat *sb, int typeflag){
	if(typeflag == FTW_F){
		if(sb->st_size <= aInt || aInt == -1){
			aInt = sb->st_size;
			aString = realloc(aString, strlen(fpath) +1);
			aString[strlen(fpath)] = '\0';
			aString = strncpy(aString, fpath, strlen(fpath));
		}
	}
	return 0;
}

//helper function for finding largest directory
int largestDir(const char *fpath, const struct stat *sb, int typeflag){
	if(typeflag == FTW_D){
		int dirSize = 0;

		DIR *dirP;
		if((dirP = opendir(fpath)) == NULL){
				perror("Error opening directory");
				exit(-1);
		}

		struct dirent *dir;
		while((dir = readdir(dirP)) != NULL){
			if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){
				if(dir->d_type == DT_REG){
					char filePath[ARGLEN];
					strcpy(filePath, fpath);
					strcat(filePath, "/");
					strcat(filePath, dir->d_name);

					struct stat stats;
					if(lstat(filePath, &stats)){
						perror("Error getting stats");
						exit(-1);
					}
					dirSize += stats.st_size;
				}
			}
		}
		closedir(dirP);

		if(dirSize >= aInt){
			aInt = dirSize;
			aString = realloc(aString, strlen(fpath) +1);
			aString[strlen(fpath)] = '\0';
			aString = strncpy(aString, fpath, strlen(fpath));
		}
	}
	return 0;
}

//helper function for finding smallest directory
int smallestDir(const char *fpath, const struct stat *sb, int typeflag){
	if(typeflag == FTW_D){
		int dirSize = 0;

		DIR *dirP;
		if((dirP = opendir(fpath)) == NULL){
				perror("Error opening directory");
				exit(-1);
		}

		struct dirent *dir;
		while((dir = readdir(dirP)) != NULL){
			if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){
				if(dir->d_type == DT_REG){
					char filePath[ARGLEN];
					strcpy(filePath, fpath);
					strcat(filePath, "/");
					strcat(filePath, dir->d_name);

					struct stat stats;
					if(lstat(filePath, &stats)){
						perror("Error getting stats");
						exit(-1);
					}
					dirSize += stats.st_size;
				}
			}
		}
		closedir(dirP);

		if(dirSize <= aInt || aInt == -1){
			aInt = dirSize;
			aString = realloc(aString, strlen(fpath) +1);
			aString[strlen(fpath)] = '\0';
			aString = strncpy(aString, fpath, strlen(fpath));
		}
	}
	return 0;
}

//helper function for finding largest tree
int largestTree(const char *fpath, const struct stat *sb, int typeflag){
	char *path = malloc(sizeof(char) * strlen(fpath) +1);
	path = strncpy(path, fpath, strlen(fpath));
	path[strlen(fpath)] = '\0';
	if(typeflag == FTW_D){
		int treeSize = getDirStats(path).size;
		if(treeSize >= aInt){
			aInt = treeSize;
			aString = realloc(aString, strlen(path));
			aString = strncpy(aString, path, strlen(path));
		}
	}
	free(path);
	return 0;
}

//helper function for finding smallest tree
int smallestTree(const char *fpath, const struct stat *sb, int typeflag){
	char *path = malloc(sizeof(char) * strlen(fpath) +1);
	path = strncpy(path, fpath, strlen(fpath));
	path[strlen(fpath)] = '\0';
	if(typeflag == FTW_D){
		int treeSize = getDirStats(path).size;
		if(treeSize <= aInt || aInt == -1){
			aInt = treeSize;
			aString = realloc(aString, strlen(path));
			aString = strncpy(aString, path, strlen(path));
		}
	}
	free(path);
	return 0;
}

//print summary of files found in directory
void summaryTable(char *dirPath){
	//number of types of files found
	int files = 0;
	int dirs = 0;
	int charDevs = 0;
	int blockDevs = 0;
	int fifos = 0;
	int links = 0;
	int sockets = 0;
	int unknowns = 0;

	//sum of sizes of types of files
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

			//check what type of file it is
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

	//print summary for types of files that were found
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

//used to recursively print the decision tree for debugging purposes
void printTree(struct node *root, int depth){
	if(root->type != NONE){
		char str[64] = "";
		int i;
		for(i=0; i<depth; i++){
			strcat(str, "\t");
		}
		if(root->type == BRANCH){
			switch(root->operate){
				case AND:
					strcat(str, "AND");
					break;
				case OR:
					strcat(str, "OR");
					break;
				case NOT:
					strcat(str, "NOT");
					break;
				default:
					strcat(str, "I don't know.");
					break;
			}
			printf("%s\n", str);
			printTree(root->left, depth+1);
			if(root->operate == AND || root->operate == OR)
				printTree(root->right, depth+1);
		}
		else{
			strcat(str, root->expression);
			strcat(str, ": ");
			strcat(str, root->option);
			printf("%s\n", str);
		}
	}
}

//recursively search through directories
void printDir(char *directory, struct node *expressionRoot, BOOL dirs){
	if(dirs)
		printf("%s\n", directory);

	DIR *currentDir;
	if((currentDir = opendir(directory)) == NULL){
			perror("Error opening directory");
			exit(-1);
	}

	struct dirent *dir;
	while((dir = readdir(currentDir)) != NULL){
		if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){
			char subDir[ARGLEN];
			strcpy(subDir, directory);
			strcat(subDir, "/");
			strcat(subDir, dir->d_name);

			if(dir->d_type == DT_DIR){
				printDir(subDir, expressionRoot, dirs);
			}
			else{
				if(parseTree(expressionRoot, subDir)){
					//printf("found good file\n");
					printf("%s/%s\n", directory, dir->d_name);
				}
			}
		}
	}
	closedir(currentDir);
}

//pint directory tree
void treeDir(char *dirPath, char *dirName,  int depth){
	char dirStr[64] = "";
	int i;
	//make string of lines depending on depth
	if(depth > 0){
		for(i=0; i<depth-1; i++){
			strcat(dirStr, "| ");
		}
		strcat(dirStr, "+-");
	}
	strcat(dirStr, dirName);

	//get directory stats
	struct dirStats stats = getDirStats(dirPath);

	//print lines, directory, and stats
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

//get number of files, number of directories, and size of directory
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

//helper function to recursively get size of directory
int sizeSum(const char *fpath, const struct stat *sb, int typeflag){
	sum += sb->st_size;
	return 0;
}

//parse decision tree
BOOL parseTree(struct node *root, char *filePath){
	if(root->type == BRANCH){
		if(root->operate == AND)
			return (parseTree(root->left, filePath) && parseTree(root->right, filePath));
		else if(root->operate == OR)
			return (parseTree(root->left, filePath) || parseTree(root->right, filePath));
		else if(root->operate == NOT)
			return (!parseTree(root->left, filePath));
	}
	else{
		//check data to see what about the file to test
		if(!strcmp(root->expression, "-access"))
			return hasAccess(filePath, root->option);
		else if(!strcmp(root->expression, "-size"))
			return size(filePath, root->option);
		else if(!strcmp(root->expression, "-print"))
			return TRUE;
		else if(!strcmp(root->expression, "-sparse"))
			return sparse(filePath);
	}
	printf("I don't know what %s means \n", root->expression);
	exit(-1);
}

//make expression tree from list of expressions
struct node *makeTree(char **expressions, int nExpressions){
	if(nExpressions == 0){
		printf("no expressions\n");
		struct node *root = malloc(sizeof(struct node));
		root->type = NONE;
		return root;
	}

	//find ors and split by them first
	int i = 0;
	int orLoc = -1;
	for(i=0; i<nExpressions; i++){
		if(!strcmp(expressions[i], "-or"))
			orLoc = i;
	}

	//or found, make or node
	if(orLoc != -1){
		struct node *root = malloc(sizeof(struct node));
		root->type = BRANCH;
		root->operate = OR;
		root->left = makeTree(expressions, orLoc);
		root->right = makeTree(expressions + orLoc+1, nExpressions-orLoc);
		return root;
	}

	//if no or's are found, split by ands
	int andLoc = -1;
	for(i=0; i<nExpressions; i++){
		if(!strcmp(expressions[i], "-and"))
			andLoc = i;
	}

	//and found, make and node
	if(andLoc != -1){
		struct node *root = malloc(sizeof(struct node));
		root->type = BRANCH;
		root->operate = AND;
		root->left = makeTree(expressions, andLoc);
		root->right = makeTree(expressions + orLoc+1, nExpressions-andLoc);
		return root;
	}

	//no or or and found
	int realNExpressions = 0;
	for(i=0; i<nExpressions; i++){
		//check if the expression needs an additional argument
		if(!(!strcmp(expressions[i], "-not") || !strcmp(expressions[i], "-size") || 
			!strcmp(expressions[i], "-atime") || !strcmp(expressions[i], "-uid") || 
			!strcmp(expressions[i], "-smallest") || !strcmp(expressions[i], "-largest") ||
			!strcmp(expressions[i], "-access"))){

			realNExpressions++;
		}
	}

	//if there's actually only one expression (including options for that expressions)
	if(realNExpressions == 1){
		struct node *root = malloc(sizeof(struct node));
		if(!strcmp(expressions[0], "-not")){
			root->type = BRANCH;
			root->operate = NOT;

			struct node *expr = malloc(sizeof(struct node));
			expr->type = LEAF;
			expr->expression = expressions[1];
			if(nExpressions == 3)
				expr->option = expressions[2];

			root->left = expr;
			return root;
		}

		root->type = LEAF;
		root->expression = expressions[0];
		if(nExpressions == 2)
			root->option = expressions[1];
		return root;
	}
	else{
		struct node *root = malloc(sizeof(struct node));
		root->type = BRANCH;
		root->operate = AND;

		int realExpressionsSoFar = 0;
		for(i=0; i<nExpressions && realExpressionsSoFar != realNExpressions/2; i++){
			if(!(!strcmp(expressions[i], "-not") || !strcmp(expressions[i], "-size") || 
				!strcmp(expressions[i], "-atime") || !strcmp(expressions[i], "-uid") || 
				!strcmp(expressions[i], "-smallest") || !strcmp(expressions[i], "-largest") ||
				!strcmp(expressions[i], "-access"))){

				realExpressionsSoFar++;
			}
		}

		root->left = makeTree(expressions, i);
		root->right = makeTree(expressions + i+1, nExpressions-i);
		return root;
	}
}
