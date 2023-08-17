#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm_translator.h"

#define BUFSIZE 512
#define MAXTOKS 256

void push(FILE *fp, Expr *n);
void pop(FILE *fp, Expr *n);
void set(FILE *fp, Expr *n);
void end(FILE *fp, Expr *n);
void add(FILE *fp, Expr *n);
void sub(FILE *fp, Expr *n);
void neg(FILE *fp, Expr *n);
void eq(FILE *fp, Expr *n);
void gt(FILE *fp, Expr *n);
void lt(FILE *fp, Expr *n);
void and(FILE *fp, Expr *n);
void or(FILE *fp, Expr *n);
void not(FILE *fp, Expr *n);
void pop2(FILE *fp);

KeyVal ops[] = {
	{"push", CMD_PUSH},
	{"pop", CMD_POP},	
	{"set", CMD_SET},	
	{"end", CMD_END},	
	{"add", CMD_ADD},	
	{"sub", CMD_SUB},	
	{"neg", CMD_NEG},	
	{"eq", CMD_EQ},	
	{"gt", CMD_GT},	
	{"lt", CMD_LT},	
	{"and", CMD_AND},	
	{"or", CMD_OR},	
	{"not", CMD_NOT},	
};

KeyVal segs[] = {
	{"sp", SEG_SP},
	{"local", SEG_LCL},
	{"argument", SEG_ARG},
	{"pointer", SEG_PTR},
	{"this", SEG_THIS},
	{"that", SEG_THAT},
	{"temp", SEG_TEMP},
	{"static", SEG_STAT},
	{"constant", SEG_CONST},
};

void (* cmds[])(FILE *fp, Expr *n) = {
	&push,
	&pop,
	&set,
	&end,
	&add,
	&sub,
	&neg,
	&eq,
	&gt,
	&lt,
	&and,
	&or,
	&not
};

char line[128];

int main(int argc, char **argv){
	FILE *fp;
	char fn[64];
	char *p;
	ExprList *exprList;
	Expr *expr;
	if(argc < 2){
		fprintf(stderr, "Error: Not enough arguments\n");
		fprintf(stderr, "Usage: ./vm_translator file-name.vm\n");
		exit(1);	
	}	
	
	strncpy(fn, argv[1], 64);
	fp = fopen(fn, "r");
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
	
	fclose(fp);
	
	p = strrchr(fn, '.');	
	p++;
	strncpy(p, "asm", 3);
	
	fp = fopen(fn, "w");
	if(fp == NULL){
		fprintf(stderr, "Error: failed to open output file\n");
		deleteExprList(exprList);
		exit(1);
	}

	writeExprs(fp, exprList);
	
	deleteExprList(exprList);
}

void writeExprs(FILE *fp, ExprList *l){
	Expr *n;

	for(n = l->head; n != NULL; n = n->next){
		cmds[n->cmd](fp, n);
	}
}

void procToks(int tokc, char **toks, ExprList *l){
	KeyVal op;
	int i, nOps, nSegs;	
	enum CmdType cmd;
	enum SegType seg;
	uint16_t val;

	seg = SEG_NULL;
	val = 0;

	nOps = sizeof(ops) / sizeof(KeyVal);
	nSegs = sizeof(segs) / sizeof(KeyVal);
	for(i = 0; i < nOps; i++){
		if(!strcmp(toks[0], ops[i].key)){
			cmd = ops[i].val;
			break;
		}
	}	
	
	if(cmd == CMD_PUSH || cmd == CMD_POP || cmd == CMD_SET){
		for(i = 0; i < nSegs; i++){
			if(!strcmp(toks[1], segs[i].key)){
				seg = segs[i].val;
				break;
			}
		}
		val = atoi(toks[2]);
	}
	
	addExpr(l, cmd, seg, val);
}

void parse(ExprList *l, FILE *fp){
	char buf[BUFSIZE];
	char *p, *c;
	char *toks[MAXTOKS];
	int len, i, n;

	while(fgets(buf, BUFSIZE, fp) != NULL){
		p = strchr(buf, '\n');
		if(p != NULL) *p = '\0';
		p = strchr(buf, '\r');
		if(p != NULL) *p = '\0';
		len = strlen(buf);
		for(i = 0; i < len; i++){
			if(buf[i] != ' ' && buf[i] != '\t'){
				p = &buf[i];
				break;
			}
		}
		if(i == len) continue;
		if(p[0] == '/' && p[1] == '/') continue;
		c = strchr(p, '/');
		if(c != NULL){
			if(*(c+1) == '/') *p = '\0';
		}
		
		n = tokenize(p, toks, MAXTOKS);
		if(n == 0 || (n == 1 && toks[0] == NULL)) continue;
		procToks(n, toks, l);
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
			toks[i] = strtok(NULL, " \t");
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

void push(FILE *fp, Expr *n){
	if(n->seg == SEG_CONST){
		sprintf(line, "@%d\nD=A\n", n->val);
		fwrite(line, strlen(line), 1, fp);
	}else{
		sprintf(line, "@%d\n", n->seg);
		fwrite(line, strlen(line), 1, fp);
		
		if(n->val != 0){
			sprintf(line, "D=M\n@%d\nA=D+A\n", n->val);
			fwrite(line, strlen(line), 1, fp);
		}else{
			sprintf(line, "A=M\n");
			fwrite(line, strlen(line), 1, fp);
		}

		sprintf(line, "D=M\n");
		fwrite(line, strlen(line), 1, fp);
	}

	sprintf(line, "@SP\nA=M\nM=D\n@SP\nM=M+1\n");
	fwrite(line, strlen(line), 1, fp);
}

void pop(FILE *fp, Expr *n){
	sprintf(line, "@%d\nD=M\n", n->seg);
	fwrite(line, strlen(line), 1, fp);

	if(n->val != 0){
		sprintf(line, "@%d\nD=D+A\n", n->val);
		fwrite(line, strlen(line), 1, fp);
	}

	sprintf(line, "@R13\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n");
	fwrite(line, strlen(line), 1, fp);
}

void set(FILE *fp, Expr *n){
	sprintf(line, "@%d\nD=A\n@%d\nM=D\n", n->val, n->seg);
	fwrite(line, strlen(line), 1, fp);
}

void end(FILE *fp, Expr *n){
	sprintf(line, "(EXIT)\n@EXIT\n0;JMP\n");
	fwrite(line, strlen(line), 1, fp);
}

void add(FILE *fp, Expr *n){
	sprintf(line, "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nD=M+D\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
	fwrite(line, strlen(line), 1, fp);
}

void sub(FILE *fp, Expr *n){
	sprintf(line, "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nD=M-D\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
	fwrite(line, strlen(line), 1, fp);
}

void neg(FILE *fp, Expr *n){
	sprintf(line, "@SP\nM=M-1\nA=M\nM=-M\n");
	fwrite(line, strlen(line), 1, fp);
}

void eq(FILE *fp, Expr *n){
	static int eq = 0;

	pop2(fp);

	sprintf(line, "D=M-D\n@IF_EQ_%d\nD;JEQ\n@SP\nA=M\nM=0\n", eq);
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "@END_IF_EQ_%d\n0;JMP\n", eq);
	fwrite(line, strlen(line), 1, fp);
        
	sprintf(line, "(IF_EQ_%d)\n@SP\nA=M\nM=-1\n", eq);
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "(END_IF_EQ_%d)\n@SP\nM=M+1\n", eq);
	fwrite(line, strlen(line), 1, fp);

	eq++;
}

void gt(FILE *fp, Expr *n){
	static int gt = 0;
	
	pop2(fp);

	sprintf(line, "D=M-D\n@IF_GT_%d\nD;JGT\n@SP\nA=M\nM=0\n", gt);
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "@END_IF_GT_%d\n0;JMP\n", gt);
	fwrite(line, strlen(line), 1, fp);
        
	sprintf(line, "(IF_GT_%d)\n@SP\nA=M\nM=-1\n", gt);
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "(END_IF_GT_%d)\n@SP\nM=M+1\n", gt);
	fwrite(line, strlen(line), 1, fp);

	gt++;
}

void lt(FILE *fp, Expr *n){
	static int lt = 0;

	pop2(fp);

	sprintf(line, "D=M-D\n@IF_LT_%d\nD;JLT\n@SP\nA=M\nM=0\n", lt);
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "@END_IF_LT_%d\n0;JMP\n", lt);
	fwrite(line, strlen(line), 1, fp);
        
	sprintf(line, "(IF_LT_%d)\n@SP\nA=M\nM=-1\n", lt);
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "(END_IF_LT_%d)\n@SP\nM=M+1\n", lt);
	fwrite(line, strlen(line), 1, fp);

	lt++;
}

void and(FILE *fp, Expr *n){
	pop2(fp);

	sprintf(line, "D=D&M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
	fwrite(line, strlen(line), 1, fp);
}

void or(FILE *fp, Expr *n){
	pop2(fp);

	sprintf(line, "D=D|M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
	fwrite(line, strlen(line), 1, fp);
}

void not(FILE *fp, Expr *n){
	sprintf(line, "@SP\nM=M-1\nA=M\nM=!M\n@SP\nM=M+1\n");
	fwrite(line, strlen(line), 1, fp);
}

void pop2(FILE *fp){
	sprintf(line, "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\n");
	fwrite(line, strlen(line), 1, fp);
}
