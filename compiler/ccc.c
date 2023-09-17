#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "codegen.h"
#include "vmt.h"

int frontEnd(char *intermediateDir, char *CmmFn);
int backEnd(char *intermediateDir, char *AsmFn);

int main(int argc, char **argv){
	char fn[64];
	char dir[64];
	char *p;

	if(argc < 2){
		fprintf(stderr, "Usage: ./ccc filename.cmm\n");	
        fprintf(stderr, "file-name.cmm: C-- file containing the source program\n");
		exit(1);
	}
	
	strcpy(fn, argv[1]);
	p = strrchr(fn, '.');
	if(p != NULL) *p = '\0';
	strcpy(dir, "i_");
	strcat(dir, fn);
	mkdir(dir, 0777);

	frontEnd(dir, argv[1]);		

	strcat(fn, ".asm");

	backEnd(dir, fn);

	exit(0);
}
int frontEnd(char *intermediateDir, char *CmmFn){
	char *fnout, *p;
	char outFunc[64];
	char fn[64];
	FILE *fp;
	TokList *TokL;
	AST* ast;

	// TODO: implement function support
	// TODO: Clean up variable usage
	
	fnout = "main.vm";
	strncpy(outFunc, fnout, 4);
	outFunc[4] = '\0';

	printf("Opening file: %s\n", CmmFn);
	fp = fopen(CmmFn, "r");
	if(fp == NULL){
		perror("Failed to open input file");
		return -1;
	}

	TokL = newTokList();
	if(TokL == NULL){
		fprintf(stderr, "Failed to create token list\n");
		fclose(fp);
		return -1;
	}

	VarL = newVarList();
	if(VarL == NULL){
		fprintf(stderr, "Failed to create variable list\n");
		fclose(fp);
		deleteTokList(TokL);
		return -1;
	}
	
	// SCAN
	if(scan(fp, TokL) == -1){
		fclose(fp);
		deleteTokList(TokL);
		deleteVarList(VarL);
		return -1;
	}
	fclose(fp);
	
	// PARSE
	ast = parse(TokL);
	if(ast == NULL){
		deleteTokList(TokL);
		deleteVarList(VarL);
		return -1;
	}
	deleteTokList(TokL);

	// VM OUT
	chdir(intermediateDir);
	if(setupVM(outFunc) == -1){
		deleteAST(ast);
		deleteVarList(VarL);
		return -1;
	}
	if(generateVM(fnout, outFunc, ast) == -1){
		deleteAST(ast);
		deleteVarList(VarL);
		return -1;	
	}

	deleteVarList(VarL);
	deleteAST(ast);
	chdir("..");

	return 0;
}

int backEnd(char *intermediateDir, char *AsmFn){
	char *p;
	char fn[64];
	char files[64][64];
	int i, nFiles;
	struct dirent *dp;
	DIR *dfd;

	if((dfd = opendir(intermediateDir)) == NULL){
		perror("Failed to open directory");
		return -1;
	}
	
	nFiles = 0;
	// Clean directory and find .vm files
	while((dp = readdir(dfd)) != NULL){
		if(strlen(dp->d_name) > 3){
			p = strrchr(dp->d_name, '.');
			if(p != NULL){
				if(!strncmp(p, ".vm", 3)){
					if(strcmp(dp->d_name, "sys.vm") && strcmp(dp->d_name, "main.vm")){
						strncpy(files[nFiles++], dp->d_name, 64);
					}
				} 
				if(!strncmp(p, ".i", 2) || !strncmp(p, ".asm", 4) || !strncmp(p, ".hack", 5)){
					printf("Removing file: %s\n", dp->d_name);
					sprintf(fn, "%s/%s", intermediateDir, dp->d_name);
					remove(fn);
				}
			}
		}
	}

	translateVmFile(intermediateDir, "sys.vm");
	translateVmFile(intermediateDir, "main.vm");

	for(i=0; i < nFiles; i++){
		translateVmFile(intermediateDir, files[i]);
	}

	assembleFinal(AsmFn, intermediateDir, files, nFiles);

	return 0;
}
