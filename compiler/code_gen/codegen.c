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
	int ret, i, nLocals;
	char *fnin, *fnout, *p;
	char outFunc[64];
	FILE *fp;
	TokList *TokL;
	AST *ast;

	if(argc < 3){
		fprintf(stderr, "Usage: ./codegen filename.txt filename.vm\n");	
        fprintf(stderr, "file-name.txt: file containing the source program\n");
        fprintf(stderr, "file-name.vm: output file containing VM commands");
		exit(1);
	}
	
	fnin = argv[1];
	fnout = argv[2];
	strncpy(outFunc, fnin, 64);
	p = strrchr(outFunc, '.');
	if(p != NULL) *p = '\0';
	
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
	ast = parse(TokL);
	if(ast == NULL){
		exit(2);
	}

	// AST
	// VM OUT
	
	deleteAST(ast);
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

	new->program = program();
	if(new->program == NULL){
		fprintf(stderr, "Failed to create program\n");
		return NULL;
	}

	return new;
}

Program *program(){
	Program *p;

	puts("Creating program");
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
		fprintf(stderr, "No EOF found\n");
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
	
	cs->statements = getStatements();

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
	strncpy(a->left, current->name, 64);
	//printf("found: %s = ", a->left);
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
	//puts("");
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
		//printf("+ ");
		e->e = expression();
		if(e->e == NULL) return NULL;
	}else if(current->tok == MINUS){
		e->op = OP_MINUS;	
		consume(MINUS);
		//printf("- ");
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
		//printf("* ");
	}else if(current->tok == DIV){
		t->op = OP_DIV;
		consume(DIV);
		if(t->t == NULL) return NULL;
		//printf("/ ");
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

	if(current->tok == LPAREN){
		consume(LPAREN);

		f->e = expression();
		if(f->e == NULL){
			return NULL;
		}

		if(!consume(RPAREN)){
			fprintf(stderr, "Expected )\n");
			return NULL;
		}
	}else if(current->tok == PLUS){
		consume(PLUS);
		f->type = D_UNARY;
		f->op = OP_PLUS;
		f->f = factor();
		if(f->f == NULL){
			return NULL;
		}	
	}else if(current->tok == MINUS){
		consume(MINUS);
		f->type = D_UNARY;
		f->op = OP_MINUS;
		f->f = factor();
		if(f->f == NULL){
			return NULL;
		}
	}else if(current->tok == INT){
		f->type = D_INT;
		strncpy(f->data, current->name, 64);
		consume(INT);
		//printf("%s ", f->data);
	}else if(current->tok == ID){
		f->type = D_VAR;
		strncpy(f->data, current->name, 64);
		consume(ID);
		//printf("%s ", f->data);
	}else{
		fprintf(stderr, "Invalid Syntax\n");
		return NULL;
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
	deleteProgram(ast->program);
	free(ast);
}

void deleteProgram(Program *program){
	if(program == NULL) return;
	deleteCompoundStatement(program->cs);
	free(program);
}

void deleteCompoundStatement(CompoundStatement *cs){
	if(cs == NULL) return;
	deleteStatementList(cs->statements);
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

void visitCompoundStatement(CompoundStatement *cs){

}
	
void visitStatement(Statement *s){
	
	}
	
void visitAssignmentStatement(AssignmentStatement *as){
	
	}
	
void visitExpression(Expression *e){
	
	}
	
void visitTerm(Term *t){
	
	}
	
void visitFactor(Factor *f){

}
