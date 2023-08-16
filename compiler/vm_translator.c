#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm_translator.h"

#define BUFSIZE 512
#define MAXTOKS 256

int main(int argc, char **argv){
	FILE *fp;
	ExprList *exprList;
	Expr *expr;
	if(argc < 2){
		fprintf(stderr, "Error: Not enough arguments\n");
		fprintf(stderr, "Usage: ./vm_translator file-name.vm\n");
		exit(1);	
	}	
	
	fp = fopen(argv[1], "r");
	if(fp == NULL){
		perror("Failed to open file");	
		exit(1);
	}
	exprList = newExprList();
	if(exprList == NULL){
		perror("Failed to allocate memory for expression list");
		fclose(fp);
		exit(1);
	}
	
	parse(exprList, fp);
	
	for(expr = exprList->head; expr != NULL; expr = expr->next){
		printf("%d, %d, %d\n", expr->cmd, expr->seg, expr->val);
	}
	
	fclose(fp);
	deleteExprList(exprList);
}

void parse(ExprList *l, FILE *fp){
	char buf[BUFSIZE];
	char *p, *c;
	char *toks[MAXTOKS];
	int len, i, n;

	while(fgets(buf, BUFSIZE, fp) != NULL){
		// Remove newline and carriage return
		p = strchr(buf, '\n');
		if(p != NULL) *p = '\0';
		p = strchr(buf, '\r');
		if(p != NULL) *p = '\0';
		// Ignore leading whitespace from line
		len = strlen(buf);
		for(i = 0; i < len; i++){
			if(buf[i] != ' ' && buf[i] != '\t'){
				p = &buf[i];
				break;
			}
		}
		// Skip empty lines		
		if(i == len) continue;
		// Skip comment lines
		if(p[0] == '/' && p[1] == '/') continue;
		c = strchr(p, '/');
		if(c != NULL){
			if(*(c+1) == '/') *p = '\0';
		}
		
		n = tokenize(p, toks, MAXTOKS);
		
	}
	
}

int tokenize(char *s, char *toks[], int max){
	int i;

	i = 0;
	toks[i] = strtok(s, " \t");
	while(toks[i++] != NULL){
		if(i >= max - 1){
			toks[i] = NULL;
		}else{
			toks[i] = strtok(s, " \t");
		}
	}
	return i;
}
ExprList *newExprList(){
	ExprList *l;
	l = malloc(sizeof(ExprList));
	if(l == NULL){
		return NULL;
	}
	
	l->head = NULL;

	return l;
}

int deleteExprList(ExprList *l){
	Expr *p, *n;
	int i;

	if(l == NULL) return -1;
	for(p = l->head, i = 0; p != NULL; p = n, i++){
		n = p->next;	
		free(p);
	}	
	return i;
}

int addExpr(ExprList *l, enum CmdType cmd, enum SegType seg, uint16_t val){
	Expr *new, *p;

	if(l == NULL) return -1;
	
	new = malloc(sizeof(Expr));
	if(new == NULL){
		return -2;
	}

	new->next = NULL;
	new->cmd = cmd;
	new->seg = seg;
	new->val = val;
	
	if(l->head == NULL){
		l->head = new;
	}else{
		for(p = l->head; p->next != NULL; p = p->next);
		p->next = new;
	}
	
	return 0;
}
