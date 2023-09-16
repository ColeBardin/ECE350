#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "vmt.h"

#define BUFSIZE 512
#define MAXTOKS 256
#define BIN16_TO_15(x) (x & 0x7FFF)

void push(FILE *fp, Expr *n);
void pop(FILE *fp, Expr *n);
void set(FILE *fp, Expr *n);
void end(FILE *fp, Expr *n);
void add(FILE *fp, Expr *n);
void sub(FILE *fp, Expr *n);
void mult(FILE *fp, Expr *n);
void divi(FILE *fp, Expr *n);
void neg(FILE *fp, Expr *n);
void eq(FILE *fp, Expr *n);
void gt(FILE *fp, Expr *n);
void lt(FILE *fp, Expr *n);
void and(FILE *fp, Expr *n);
void or(FILE *fp, Expr *n);
void not(FILE *fp, Expr *n);
void func(FILE *fp, Expr *n);
void call(FILE *fp, Expr *n);
void ret(FILE *fp, Expr *n);
void ifgt(FILE *fp, Expr *n);
void go2(FILE *fp, Expr *n);
void label(FILE *fp, Expr *n);
void pushString(FILE *fp, char *s);
void pop2(FILE *fp);

KeyVal ops[] = {
	{"push", CMD_PUSH},
	{"pop", CMD_POP},	
	{"set", CMD_SET},	
	{"end", CMD_END},	
	{"add", CMD_ADD},	
	{"sub", CMD_SUB},	
	{"mult", CMD_MULT},	
	{"div", CMD_DIV},	
	{"neg", CMD_NEG},	
	{"eq", CMD_EQ},	
	{"gt", CMD_GT},	
	{"lt", CMD_LT},	
	{"and", CMD_AND},	
	{"or", CMD_OR},	
	{"not", CMD_NOT},	
	{"function", CMD_FUNC},	
	{"call", CMD_CALL},	
	{"return", CMD_RET},	
	{"if-goto", CMD_IF},	
	{"goto", CMD_GOTO},	
	{"label", CMD_LBL},	
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
	&mult,
	&divi,
	&neg,
	&eq,
	&gt,
	&lt,
	&and,
	&or,
	&not,
	&func,
	&call,
	&ret,
	&ifgt,
	&go2,
	&label
};

char line[128];

int main(int argc, char **argv){
	FILE *fp;
	char fn[64];
	char *p;
	char *dir;
	char files[64][64];
	int i, nFiles;
	ExprList *exprList;
	Expr *expr;
	struct dirent *dp;
	DIR *dfd;


	if(argc < 3){
        fprintf(stderr, "Usage: ./vmt2 file-name.asm path-name\n");
        fprintf(stderr, "file-name.asm: assembly file to be generated by the translator\n");
        fprintf(stderr, "path-name: directory containing .vm source files\n");
        fprintf(stderr, "Example: ./vmt2 mult-final.asm ./mult\n");
		exit(1);	
	}	

	dir = argv[2];
	
	if((dfd = opendir(dir)) == NULL){
		perror("Failed to open directory");
		exit(1);
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
					sprintf(fn, "%s/%s", dir, dp->d_name);
					remove(fn);
				}
			}
		}
	}

	translateFile(dir, "sys.vm");
	translateFile(dir, "main.vm");

	for(i=0; i < nFiles; i++){
		translateFile(dir, files[i]);
	}

	strcpy(fn, argv[1]);
	assembleFinal(fn, dir, files, nFiles);	
}

int fileAppend(FILE *dest, char *source_fn){
	FILE *fp;
	char c;

	fp = fopen(source_fn, "r");
	if(fp == NULL){
		perror("Failed to open '%s'");
		return -1;
	}
	c = fgetc(fp);
	while(c != EOF){
		fputc(c, dest);
		c = fgetc(fp);
	}
	fclose(fp);
	return 0;
}

void assembleFinal(char *fno, char *dir, char files[64][64], int nfiles){
	FILE *fpo;
	FILE *fp;
	char *p;
	int i;
	char c;
	char fn[64];

	sprintf(fn, "%s/%s", dir, fno);
	printf("Creating %s\n", fn);
	fpo = fopen(fn, "w");
	if(fpo == NULL){
		perror("Failed to open output asm file");
		return;
	}
	
	sprintf(fn, "%s/%s", dir, "sys.i");	
	if(fileAppend(fpo, fn) < 0) return;

	sprintf(fn, "%s/%s", dir, "main.i");	
	if(fileAppend(fpo, fn) < 0) return;
	
	for(i = 0; i < nfiles; i++){
		p = strrchr(files[i], '.');
		strcpy(p, ".i");
		sprintf(fn, "%s/%s", dir, files[i]);	
		if(fileAppend(fpo, fn) < 0) return;
	}
	fclose(fpo);
}

void translateFile(char *dir, char *file){
	FILE *fp;
	ExprList *exprList;
	char *p;
	char fn[64];
	
	
	sprintf(fn, "%s/%s", dir, file);
	fp = fopen(fn, "r");
	if(fp == NULL){
		fprintf(stderr, "Error: `%s` file not found in `%s` dir\n", file, dir);
		perror("");
		exit(1);
	}
	exprList = newExprList();
	parse(exprList, fp);
	fclose(fp);

	p = strrchr(fn, '.');
	strcpy(p, ".i");
	fp = fopen(fn, "w");
	writeExprs(fp, exprList);
	fclose(fp);
	deleteExprList(exprList);
}

void writeExprs(FILE *fp, ExprList *l){
	Expr *n;

	for(n = l->head; n != NULL; n = n->next){
		cmds[n->cmd](fp, n);
	}
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
		if(c != NULL) *c = '\0';
		
		n = tokenize(p, toks, " \t", MAXTOKS);
		if(n == 0 || (n == 1 && toks[0] == NULL)) continue;
		procToks(n, toks, l);
	}
	
}

void procToks(int tokc, char **toks, ExprList *l){
	KeyVal op;
	int i;
	enum CmdType cmd;
	enum SegType seg;
	uint16_t val;
	char *p;
	static int nOps = sizeof(ops) / sizeof(KeyVal);
	static int nSegs = sizeof(segs) / sizeof(KeyVal);

	seg = SEG_NULL;
	val = 0;
	p = NULL;

	for(i = 0; i < nOps; i++){
		if(!strcmp(toks[0], ops[i].key)){
			cmd = ops[i].val;
			break;
		}
	}	
	
	switch(cmd){
		case CMD_PUSH:
		case CMD_POP:
		case CMD_SET:
			for(i = 0; i < nSegs; i++){
				if(!strcmp(toks[1], segs[i].key)){
					seg = segs[i].val;
					val = BIN16_TO_15(atoi(toks[2]));
					break;
				}
			}
			break;
		case CMD_FUNC:
		case CMD_CALL:
			p = toks[1];
			val = atoi(toks[2]);
			break;
		case CMD_IF:
		case CMD_GOTO:
		case CMD_LBL:
			p = toks[1];
			break;
		default:
			break;
	}

	addExpr(l, cmd, seg, val, p);
}

int tokenize(char *s, char *toks[], char *delim, int max){
	int i;

	i = 0;
	toks[i] = strtok(s, delim); 
	while(toks[i++] != NULL){
		if(i >= max - 1){
			toks[i] = NULL;
		}else{
			toks[i] = strtok(NULL, delim);
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
	free(l);
	return i;
}

int addExpr(ExprList *l, enum CmdType cmd, enum SegType seg, int val, char *name){
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
	if(name != NULL) strncpy(new->name, name, 64);
	
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
		if(n->seg == SEG_PTR){
			sprintf(line, "@%d\nD=A\n", SEG_THIS);
			fwrite(line, strlen(line), 1, fp);
		}else if(n->seg == SEG_TEMP){
			sprintf(line, "@%d\nD=A\n", n->seg);
			fwrite(line, strlen(line), 1, fp);
		}else{
			sprintf(line, "@%d\nD=M\n", n->seg);
			fwrite(line, strlen(line), 1, fp);
		}

		if(n->val != 0){
			sprintf(line, "@%d\nA=D+A\n", n->val);
			fwrite(line, strlen(line), 1, fp);
		}else{
			sprintf(line, "A=D\n");
			fwrite(line, strlen(line), 1, fp);
		}

		sprintf(line, "D=M\n");
		fwrite(line, strlen(line), 1, fp);
	}

	sprintf(line, "@SP\nA=M\nM=D\n@SP\nM=M+1\n");
	fwrite(line, strlen(line), 1, fp);
}

void pop(FILE *fp, Expr *n){
	if(n->seg == SEG_TEMP){
		sprintf(line, "@%d\nD=A\n", n->seg);
		fwrite(line, strlen(line), 1, fp);
	}else if(n->seg == SEG_PTR){
		sprintf(line, "@%d\nD=A\n", SEG_THIS);
		fwrite(line, strlen(line), 1, fp);
	}else{
		sprintf(line, "@%d\nD=M\n", n->seg);
		fwrite(line, strlen(line), 1, fp);
	}

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
	pop2(fp);

	sprintf(line, "D=M+D\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
	fwrite(line, strlen(line), 1, fp);
}

void sub(FILE *fp, Expr *n){
	pop2(fp);

	sprintf(line, "D=M-D\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
	fwrite(line, strlen(line), 1, fp);
}

void mult(FILE *fp, Expr *n){
	static int multN = 0;

	sprintf(line, "@SP\nA=M-1\nD=M\n@END_IF_MULT_%d\nD;JGE\n", multN);
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "@SP\nA=M-1\nM=-M\n@2\nD=A\n@SP\nA=M-D\nM=-M\n");
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "(END_IF_MULT_%d)\n@SP\nA=M\nM=0\n", multN);
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "(START_LOOP_MULT_%d)\n", multN);
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "@SP\nA=M-1\nD=M\n@END_LOOP_MULT_%d\nD;JLE\n", multN);
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "@2\nD=A\n@SP\nA=M-D\nD=M\n@SP\nA=M\nM=M+D\n");
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "@SP\nA=M-1\nM=M-1\n@START_LOOP_MULT_%d\n0;JMP\n", multN);
	fwrite(line, strlen(line), 1, fp);

	sprintf(line, "(END_LOOP_MULT_%d)\n@SP\nA=M\nD=M\n@SP\nM=M-1\nA=M-1\nM=D\n", multN);
	fwrite(line, strlen(line), 1, fp);

	multN++;
}

void divi(FILE *fp, Expr *n){
	static int divN = 0;

	// TODO: write signed division

	divN++;
}

void neg(FILE *fp, Expr *n){
	sprintf(line, "@SP\nM=M-1\nA=M\nM=-M\n@SP\nM=M+1\n");
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

void func(FILE *fp, Expr *n){
	int i;
	sprintf(line, "(%s)\n", n->name);
	fwrite(line, strlen(line), 1, fp);
	for(i = 0; i < n->val; i++){
		sprintf(line, "@0\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
		fwrite(line, strlen(line), 1, fp);
	}
}

void call(FILE *fp, Expr *n){
	static int cnt = 0;
	
	// Push RET ADDR
	sprintf(line, "@RET_ADDR_%d\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", cnt);
	fwrite(line, strlen(line), 1, fp);
	// Push LCL
	sprintf(line, "@%d\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", SEG_LCL);
	fwrite(line, strlen(line), 1, fp);
	// Push ARG
	sprintf(line, "@%d\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", SEG_ARG);
	fwrite(line, strlen(line), 1, fp);
	// Push THIS
	sprintf(line, "@%d\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", SEG_THIS);
	fwrite(line, strlen(line), 1, fp);
	// Push THAT
	sprintf(line, "@%d\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", SEG_THAT);
	fwrite(line, strlen(line), 1, fp);
	// ARG = SP - 5 - n
	sprintf(line, "@SP\nD=M\n@%d\nD=D-A\n@%d\nM=D\n", 5 + n->val, SEG_ARG);
	fwrite(line, strlen(line), 1, fp);
	// LCL = SP
	sprintf(line, "@SP\nD=M\n@%d\nM=D\n", SEG_LCL);
	fwrite(line, strlen(line), 1, fp);
	go2(fp, n);
	// Print return label
	sprintf(line, "(RET_ADDR_%d)\n", cnt++);
	fwrite(line, strlen(line), 1, fp);
}

void ret(FILE *fp, Expr *n){
	// FRAME = LCL
	sprintf(line, "@%d\nD=M\n@R14\nM=D\n", SEG_LCL);
	fwrite(line, strlen(line), 1, fp);
	// RET = *(FRAME - 5)
	sprintf(line, "@R14\nD=M\n@5\nA=D-A\nD=M\n@R15\nM=D\n");
	fwrite(line, strlen(line), 1, fp);
	// *ARG = pop()
	sprintf(line, "@SP\nM=M-1\nA=M\nD=M\n@%d\nA=M\nM=D\n", SEG_ARG);
	fwrite(line, strlen(line), 1, fp);
	// SP = ARG + 1
	sprintf(line, "@%d\nD=M+1\n@SP\nM=D\n", SEG_ARG);
	fwrite(line, strlen(line), 1, fp);
	// THAT = *(FRAME - 1)
	sprintf(line, "@R14\nD=M\n@1\nA=D-A\nD=M\n@%d\nM=D\n", SEG_THAT);
	fwrite(line, strlen(line), 1, fp);
	// THIS = *(FRAME - 2)
	sprintf(line, "@R14\nD=M\n@2\nA=D-A\nD=M\n@%d\nM=D\n", SEG_THIS);
	fwrite(line, strlen(line), 1, fp);
	// ARG = *(FRAME - 3)
	sprintf(line, "@R14\nD=M\n@3\nA=D-A\nD=M\n@%d\nM=D\n", SEG_ARG);
	fwrite(line, strlen(line), 1, fp);
	// LCL = *(FRAME - 4)
	sprintf(line, "@R14\nD=M\n@4\nA=D-A\nD=M\n@%d\nM=D\n", SEG_LCL);
	fwrite(line, strlen(line), 1, fp);
	// goto RET
	sprintf(line, "@R15\nA=M\n0;JMP\n");
	fwrite(line, strlen(line), 1, fp);
}

void ifgt(FILE *fp, Expr *n){
	sprintf(line, "@SP\nM=M-1\nA=M\nD=M\n@%s\nD;JNE\n", n->name);
	fwrite(line, strlen(line), 1, fp);
}

void go2(FILE *fp, Expr *n){
	sprintf(line, "@%s\n0;JMP\n", n->name);
	fwrite(line, strlen(line), 1, fp);
}

void label(FILE *fp, Expr *n){
	sprintf(line, "(%s)\n", n->name);
	fwrite(line, strlen(line), 1, fp);
}

void pushString(FILE *fp, char *s){
	sprintf(line, "@%s\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", s);
	fwrite(line, strlen(line), 1, fp);
}

void pop2(FILE *fp){
	sprintf(line, "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\n");
	fwrite(line, strlen(line), 1, fp);
}

