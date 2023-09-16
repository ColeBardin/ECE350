#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "codegen.h"

char line[128];
VarList *VarL;
TokNode *current;
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

int main(int argc, char **argv){
	char *fnin, *fnout, *p;
	char outFunc[64];
	FILE *fp;
	TokList *TokL;
	AST* ast;

	if(argc < 3){
		fprintf(stderr, "Usage: ./codegen filename.txt filename.vm\n");	
        fprintf(stderr, "file-name.txt: file containing the source program\n");
        fprintf(stderr, "file-name.vm: output file containing VM commands\n");
		exit(1);
	}
	
	fnin = argv[1];
	fnout = argv[2];
	strncpy(outFunc, fnin, 64);
	p = strrchr(outFunc, '.');
	if(p != NULL) *p = '\0';
	
	printf("Opening file: %s\n", fnin);
	fp = fopen(fnin, "r");
	if(fp == NULL){
		perror("Failed to open input file");
		exit(1);
	}

	TokL = newTokList();
	if(TokL == NULL){
		fprintf(stderr, "Failed to create token list\n");
		fclose(fp);
		exit(1);
	}

	VarL = newVarList();
	if(VarL == NULL){
		fprintf(stderr, "Failed to create variable list\n");
		fclose(fp);
		deleteTokList(TokL);
		exit(1);
	}
	
	// SCAN
	if(scan(fp, TokL) == -1){
		fclose(fp);
		deleteTokList(TokL);
		deleteVarList(VarL);
		exit(2);
	}
	fclose(fp);
	
	// PARSE
	ast = parse(TokL);
	if(ast == NULL){
		deleteTokList(TokL);
		deleteVarList(VarL);
		exit(2);
	}
	deleteTokList(TokL);

	// VM OUT
	if(setupVM(outFunc) == -1){
		deleteAST(ast);
		deleteVarList(VarL);
		exit(1);
	}
	if(generateVM(fnout, outFunc, ast) == -1){
		deleteAST(ast);
		deleteVarList(VarL);
		exit(1);
	}

	deleteVarList(VarL);
	deleteAST(ast);
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
	return i;
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

	puts("Deleting Token List");
	if(l == NULL) return -1;
	for(p = l->head, i = 0; p != NULL; p = n, i++){
		n = p->next;	
		free(p);
	}	
	free(l);
	return i;
}

VarList *newVarList(){
	VarList *new;
	
	new = malloc(sizeof(VarList));
	if(new == NULL){
		perror("Failed to allocate memory for variable list");
		return NULL;
	}
	new->head = NULL;
	return new;

}

int addVar(VarList *l, char *name){
	static int cnt = 0;
	VarNode *p;
	VarNode *new;

	if(l == NULL){
		fprintf(stderr, "Error: Uninitialized list\n");	
		return -1;
	}

	if(isVar(l, name) >= 0) return 0;

	new = malloc(sizeof(VarNode));
	if(new == NULL){
		perror("Failed to allocate memory for new variable");
		return -2;
	}

	new->offset = cnt++;
	strncpy(new->name, name, 64);
	new->next = NULL;

	if(l->head == NULL){
		l->head = new;
	}else{
		for(p = l->head; p->next != NULL; p = p->next);
		p->next = new;
	}

	return cnt;
}

int isVar(VarList *l, char *name){
	VarNode *p;

	if(l == NULL){
		fprintf(stderr, "isVar error, uninitialized var list\n");
		return -1;
	}else if(l->head == NULL){
		return -2;
	}else{
		for(p = l->head; p != NULL; p = p->next){
			if(!strncmp(p->name, name, 64)) return p->offset;
		}
		return -3;
	}
}

int getVarCount(VarList *l){
	VarNode *p;
	int count;

	if(l == NULL) return 0;
	for(p = l->head, count = 0; p != NULL; p = p->next, count++);
	return count;
}

int getVarOffset(VarList *l, char *name){
	VarNode *p;
	
	if(l == NULL) return -1;
	if(l->head == NULL) return -2;

	return 0;
}

int deleteVarList(VarList *l){
	VarNode*p, *n;
	int i;

	puts("Deleting Variable List");
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

	puts("Scanning for tokens");
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
					addTok(list, ID, exp);
					addVar(VarL, exp);
				}else{
					addTok(list, INT, exp);
				}
			}
		}
	}	
	addTok(list, END, "EOF");
	return 0;
}

AST *parse(TokList *list){
	AST *new;

	puts("Creating AST");
	if(list == NULL){
		fprintf(stderr, "Parse received uninitialized token list\n");
		return NULL;
	}
	if(list->head == NULL){
		fprintf(stderr, "Parse received empty token list\n");
		return NULL;
	}

	current = list->head;

	new = malloc(sizeof(AST));
	if(new == NULL){
		perror("AST");
		return NULL;
	}

	new->p = program();
	if(new->p == NULL){
		fprintf(stderr, "Failed to create program\n");
		return NULL;
	}

	return new;
}

Program *program(){
	Program *p;

	p = malloc(sizeof(Program));
	if(p == NULL){
		perror("Program");
		return NULL;
	}	

	p->cs = compoundStatement();
	if(p->cs == NULL){
		fprintf(stderr, "Failed to create Compound Statement\n");
		return NULL;
	}

	if(!consume(END)){
		fprintf(stderr, "Expected EOF\n");
		return NULL;
	}

	return p;
}

CompoundStatement *compoundStatement(){
	CompoundStatement *cs;

	cs = malloc(sizeof(CompoundStatement));
	if(cs == NULL){
		perror("Compound Statement");
		return NULL;
	}

	if(!consume(LCURLY)){
		fprintf(stderr, "Missing {\n");
		return NULL;
	}
	
	cs->sl = getStatements();

	if(!consume(RCURLY)){
		fprintf(stderr, "Missing }\n");
		return NULL;
	}
	return cs;
}

StatementList *getStatements(){
	StatementList *sl;
	Statement *s;
	
	sl = malloc(sizeof(StatementList));
	if(sl == NULL){
		perror("Statement List");
		return NULL;
	}
	sl->head = NULL;
	sl->tail = NULL;

	s = statement();
	sl->head = s;
	sl->tail = s;
	while(s != NULL){
		s = statement();
		if(s == NULL) break;
		sl->tail->next = s;	
		sl->tail = s;
	}

	return sl;
}

Statement *statement(){
	Statement *s;

	s = malloc(sizeof(Statement));
	if(s == NULL){
		perror("Statement");
		return NULL;
	}

	s->next = NULL;
	s->cs = NULL;
	s->as = NULL;

	if(current->tok == LCURLY){
		s->type = CMP_STATE;
		s->cs = compoundStatement();
	}else if(current->tok == ID){
		s->type = ASSGN_STATE;
		s->as = assignmentStatement();
	}else{
 		return NULL;
	}

	return s;
}

AssignmentStatement *assignmentStatement(){
	AssignmentStatement *a;
	
	a = malloc(sizeof(AssignmentStatement));
	if(a == NULL){
		perror("Assignment Statement\n");
		return NULL;
	}
	strncpy(a->lval, current->name, 64);
	if(!consume(ID)){
		fprintf(stderr, "Expected variable\n");
		return NULL;
	}

	if(!consume(ASSIGN)){
		fprintf(stderr, "Expected assignment operator\n");
		return NULL;
	}
	
	a->e = expression();
	if(a->e == NULL){
		return NULL;
	}

	if(!consume(SEMI)){
		fprintf(stderr, "Expected semicolon\n");
		return NULL;
	}
	return a;
}

Expression *expression(){
	Expression *e;
	
	e = malloc(sizeof(Expression));
	if(e == NULL){
		perror("Expression\n");
		return NULL;
	}
	
	e->t = term();
	if(e->t == NULL){
		return NULL;
	}

	if(current->tok == PLUS){
		e->op = OP_PLUS;
		consume(PLUS);
		e->e = expression();
		if(e->e == NULL) return NULL;
	}else if(current->tok == MINUS){
		e->op = OP_MINUS;	
		consume(MINUS);
		e->e = expression();
		if(e->e == NULL) return NULL;
	}else{
		e->op = OP_NULL;
		e->e = NULL;
	}
	
	return e;
}

Term *term(){
	Term *t;

	t = malloc(sizeof(Term));
	if(t == NULL){
		perror("Term");
		return NULL;
	}
	
	t->f = factor();
	if(current->tok == MULT){
		t->op = OP_MULT;
		consume(MULT);
		t->t = term();
		if(t->t == NULL) return NULL;
	}else if(current->tok == DIV){
		t->op = OP_DIV;
		consume(DIV);
		if(t->t == NULL) return NULL;
	}else{
		t->op = OP_NULL;
		t->t = NULL;
	}

	return t;
}

Factor *factor(){
	Factor *f;
	
	f = malloc(sizeof(Factor));
	if(f == NULL){
		perror("Factor");
		return NULL;
	}

	f->e = NULL;
	f->op = OP_NULL;
	f->f = NULL;

	switch(current->tok){
		case LPAREN:
			consume(LPAREN);

			f->e = expression();
			if(f->e == NULL){
				return NULL;
			}

			if(!consume(RPAREN)){
				fprintf(stderr, "Expected )\n");
				return NULL;
			}
			break;
		case PLUS:
			consume(PLUS);
			f->type = D_UNARY;
			f->op = OP_PLUS;
			f->f = factor();
			if(f->f == NULL){
				return NULL;
			}	
			break;
		case MINUS:
			consume(MINUS);
			f->type = D_UNARY;
			f->op = OP_MINUS;
			f->f = factor();
			if(f->f == NULL){
				return NULL;
			}
			break;
		case INT:
			f->type = D_INT;
			strncpy(f->data, current->name, 64);
			consume(INT);
			break;
		case ID:
			f->type = D_VAR;
			strncpy(f->data, current->name, 64);
			consume(ID);
			break;
		default:
			fprintf(stderr, "Invalid Syntax\n");
			return NULL;
			break;
	}
	return f;
}

int consume(enum TokType type){
	if(current->tok != type){
		fprintf(stderr, "Syntax Error\n");
		return 0;
	}
	current = current->next;
	return 1;
}

void deleteAST(AST *ast){
	if(ast == NULL) return;
	puts("Deleting AST");
	deleteProgram(ast->p);
	free(ast);
}

void deleteProgram(Program *program){
	if(program == NULL) return;
	deleteCompoundStatement(program->cs);
	free(program);
}

void deleteCompoundStatement(CompoundStatement *cs){
	if(cs == NULL) return;
	deleteStatementList(cs->sl);
	free(cs);
}

void deleteStatementList(StatementList *sl){
	Statement *c, *n;
	if(sl == NULL) return;
	for(c = sl->head; c != NULL; c = n){
		n = c->next;
		deleteStatement(c);
	}
	free(sl);
}

void deleteStatement(Statement *s){
	if(s == NULL) return;
	deleteCompoundStatement(s->cs);
	deleteAssignmentStatement(s->as);
	free(s);
}

void deleteAssignmentStatement(AssignmentStatement *as){
	if(as == NULL) return;
	deleteExpression(as->e);	
	free(as);
}

void deleteExpression(Expression *e){
	if(e == NULL) return;
	deleteTerm(e->t);
	deleteExpression(e->e);
	free(e);	
}

void deleteTerm(Term *t){
	if(t == NULL) return;
	deleteFactor(t->f);
	deleteTerm(t->t);
	free(t);
}

void deleteFactor(Factor *f){
	if(f == NULL) return;
	deleteExpression(f->e);
	deleteFactor(f->f);	
	free(f);
}

int generateVM(char *fn, char *prog, AST *ast){
	FILE *fp;

	printf("Creating file: %s\n", fn);
	printf("Resolved %d local variables\n", getVarCount(VarL));

	fp = fopen(fn, "w");
	if(fp == NULL){
		perror("Out file");
		return -1;
	}

	snprintf(line, 128, "function %s %d\n", prog, getVarCount(VarL));
	fwrite(line, strlen(line), 1, fp);
	visitCompoundStatement(ast->p->cs, fp);
	fputs("return\n", fp);
	
	fclose(fp);

	return 0;
}

void visitCompoundStatement(CompoundStatement *cs, FILE *fp){
	Statement *s;

	for(s = cs->sl->head; s != NULL; s = s->next){
		visitStatement(s, fp);
	}
}
	
void visitStatement(Statement *s, FILE *fp){
	if(s->type == CMP_STATE) visitCompoundStatement(s->cs, fp);
	else visitAssignmentStatement(s->as, fp);
}
	
void visitAssignmentStatement(AssignmentStatement *as, FILE *fp){
	visitExpression(as->e, fp);
	popVar(fp, as->lval);
}
	
void visitExpression(Expression *e, FILE *fp){
	visitTerm(e->t, fp);
	if(e->op == OP_PLUS){
		visitExpression(e->e, fp);
		doAdd(fp);
	}else if(e->op == OP_MINUS){
		visitExpression(e->e, fp);
		doSub(fp);
	}
}
	
void visitTerm(Term *t, FILE *fp){
	visitFactor(t->f, fp);
	if(t->op == OP_MULT){
		visitTerm(t->t, fp);
		doMult(fp);
	}else if(t->op == OP_DIV){
		visitTerm(t->t, fp);
		doDiv(fp);
	}
}
	
void visitFactor(Factor *f, FILE *fp){
	switch(f->type){
		case D_EXPR:
			visitExpression(f->e, fp);
			break;
		case D_INT:
			doInt(fp, atoi(f->data));	
			break;
		case D_VAR:
			doVar(fp, f->data);
			break;
		case D_UNARY:
			doInt(fp, 0);
			visitFactor(f->f, fp);
			if(f->op == OP_PLUS) doAdd(fp);
			else if(f->op == OP_MINUS) doSub(fp);
			else fprintf(stderr, "Error visiting unary operator\n");
			break;
		default:
			break;
	}
}

int setupVM(char *prog){
	FILE *fp;
	char line[128];

	puts("Setting up VM");

	puts("Creating file: sys.vm");
	fp = fopen("sys.vm", "w");
	if(fp == NULL){
		perror("sys.vm");
		return -1;
	}
	snprintf(line, 128, "function init 0\nset sp 256\ncall main 0\nend\n");
	fwrite(line, strlen(line), 1, fp);
	fclose(fp);

	puts("Creating file: main.vm");
	fp = fopen("main.vm", "w");
	if(fp == NULL){
		perror("main.vm");
		return -1;
	}
	snprintf(line, 128, "function main 0\ncall %s 0\nreturn\n", prog);
	fwrite(line, strlen(line), 1, fp);
	fclose(fp);

	return 0;
}

void popVar(FILE *fp, char *var){
	snprintf(line, 128, "pop local %d\n", isVar(VarL, var));
	fwrite(line, strlen(line), 1, fp);
}

void doAdd(FILE *fp){
	fputs("add\n", fp);
}

void doSub(FILE *fp){
	fputs("sub\n", fp);
}

void doMult(FILE *fp){
	fputs("mult\n", fp);
}

void doDiv(FILE *fp){
	fputs("div\n", fp);
}

void doInt(FILE *fp, int num){
	snprintf(line, 128, "push constant %d\n", num);
	fwrite(line, strlen(line), 1, fp);
}

void doVar(FILE *fp, char *var){
	snprintf(line, 128, "push local %d\n", isVar(VarL, var));
	fwrite(line, strlen(line), 1, fp);
}

