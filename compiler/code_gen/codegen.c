#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "codegen.h"

KeyVal allToks[] = {
	{"INTEGER", INT},
	{"PLUS", PLUS},
	{"MINUS", MINUS},
	{"MULT", MULT},
	{"DIV", DIV},
	{"(", LPAREN},
	{")", RPAREN},
	{"{", LCURLY},
	{"}", RCURLY},
	{"=", ASSIGN},
	{";", SEMI},
	{"ID", ID},
	{"EOF", END},
};

int main(int argc, char **argv){
	int ret, i;
	char *fnin, *fnout;
	FILE *fp;
	TokList *TokL;

	if(argc < 3){
		fprintf(stderr, "Usage: ./codegen filename.txt filename.vm\n");	
        fprintf(stderr, "file-name.txt: file containing the source program\n");
        fprintf(stderr, "file-name.vm: output file containing VM commands");
		exit(1);
	}
	
	fnin = argv[1];
	fnout = argv[2];
	
	fp = fopen(fnin, "r");
	if(fp == NULL){
		perror("Failed to open input file");
		exit(1);
	}
	
	ret = scan(fp, TokL);	
	// SCAN
	// PARSE
	// AST
	// VM OUT
	
	deleteTokList(TokL);
	exit(0);
}

int scan(FILE *fp, TokList *list){
	char c;

	while((c = fgetc(fp)) != EOF){
		switch (c){
			case ' ':
			case '\t':	
			case '\n':
			case '\r':
				break;
			default:
				break;
		}
	}	
	return 0;
}

TokList *newTokList(){
	TokList *new;
	
	new = malloc(sizeof(TokList));
	if(new == NULL){
		perror("Failed to allocate memory for token list");
		return NULL;
	}
	new->head = NULL;
	return new;
}

int addTok(TokList *l, enum TokType tok, char *name){
	TokNode *p;
	TokNode *new;

	if(l == NULL){
		fprintf(stderr, "Error: Uninitialized list\n");	
		return -1;
	}
	
	new = malloc(sizeof(TokNode));
	if(new == NULL){
		perror("Failed to allocate memory for new token");
		return -2;
	}

	new->tok = tok;
	strncpy(new->name, name, 64);
	new->next = NULL;

	if(l->head == NULL){
		l->head = new;
	}else{
		for(p = l->head; p->next != NULL; p = p->next);
		p->next = new;
	}
	return 0;	
}

int deleteTokList(TokList *l){
	TokNode *p, *n;
	int i;

	if(l == NULL) return -1;
	for(p = l->head, i = 0; p != NULL; p = n, i++){
		n = p->next;	
		free(p);
	}	
	free(l);
	return i;
}




