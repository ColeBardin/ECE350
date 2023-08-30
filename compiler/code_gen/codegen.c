#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "codegen.h"

#define DB 1

KeyVal allToks[] = {
	{"+", PLUS},
	{"-", MINUS},
	{"*", MULT},
	{"/", DIV},
	{"(", LPAREN},
	{")", RPAREN},
	{"{", LCURLY},
	{"}", RCURLY},
	{"=", ASSIGN},
	{";", SEMI},
};

TokNode *current;

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

	TokL = newTokList();
	if(TokL == NULL){
		fprintf(stderr, "Failed to create token list\n");
		exit(1);
	}
	
	// SCAN
	if(scan(fp, TokL) == -1){
		exit(2);
	}
	fclose(fp);
	
	if(DB){
		TokNode *c;
		puts("Tokens:");
		for(c = TokL->head; c != NULL; c = c->next){
			printf("%d: |%s|\n", c->tok, c->name);
		}
	}

	// PARSE
	if(parse(TokL) == -1){
		exit(2);
	}
	// AST
	// VM OUT
	
	deleteTokList(TokL);
	exit(0);
}

int getVarOrInt(FILE *fp, char *dest, int size){
	char c;
	int nToks, i, n;

	i = 1;
	nToks = sizeof(allToks) / sizeof(KeyVal);

	while((c = fgetc(fp)) != EOF){
		if(c == ' ' || c == '\t' || c == '\n' || c == '\r'){
			dest[i] = '\0';
			return i;
		}else{
			for(n = 0; n < nToks; n++){
				if(c == allToks[n].key[0]){
					fseek(fp, -1L, SEEK_CUR);
					dest[i] = '\0';
					return i;
				}
			}
			if(i == size - 1){
				dest[i] = '\0';
				return -1;
			}else{
				dest[i] = c;
				i++;
			}
		}
	}
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

int scan(FILE *fp, TokList *list){
	char c;
	char exp[64];
	int nToks, i;

	nToks = sizeof(allToks) / sizeof(KeyVal);

	while((c = fgetc(fp)) != EOF){
		if(c == ' ' || c == '\t' || c == '\n' || c == '\r'){
			continue;
		}else{
			for(i = 0; i < nToks; i++){
				if(c == allToks[i].key[0]){
					addTok(list, allToks[i].val, allToks[i].key);
					break;
				}
			}
			if(i == nToks){
				exp[0] = c;
				if(getVarOrInt(fp, exp, 64) == -1){
					fprintf(stderr, "Failed parsing input file\n");
					return -1;
				}
				if(exp[0] > '9' || exp[0] < '0'){
					// Variable name
					addTok(list, ID, exp);
				}else{
					// Integer
					addTok(list, INT, exp);
				}
			}
		}
	}	
	addTok(list, END, "EOF");
	return 0;
}

int parse(TokList *list){
	if(list == NULL){
		fprintf(stderr, "Parse received uninitialized token list\n");
		return -1;
	}
	if(list->head == NULL){
		fprintf(stderr, "Parse received empty token list\n");
		return -1;
	}

	current = list->head;


}

int consume(enum TokType type){
	if(current->tok != type) return 0;
	current = current->next;
	return 1;
}
