#pragma once

#include <stdint.h>

typedef struct KeyVal KeyVal;
typedef struct TokList TokList;
typedef struct TokNode TokNode;
typedef struct VarNode VarNode;
typedef struct AST AST;
typedef struct Program Program;
typedef struct CompoundStatement CompoundStatement;
typedef struct StatementList StatementList;
typedef struct Statement Statement;
typedef struct AssignmentStatement AssignmentStatement;
typedef struct Expression Expression;
typedef struct Term Term;
typedef struct Factor Factor;

enum TokType {
	PLUS,
	MINUS,
	MULT,
	DIV,
	LPAREN,
	RPAREN,
	LCURLY,
	RCURLY,
	ASSIGN,
	SEMI,
	INT,
	ID,
	END,
};

enum StatementType {
	CMP_STATE,
	ASSGN_STATE,
};

enum OpType {
	OP_NULL,
	OP_PLUS,
	OP_MINUS,
	OP_MULT,
	OP_DIV,
};

enum DataType {
	D_NULL,
	D_EXPR,
	D_UNARY,
	D_INT,
	D_VAR,
};

struct KeyVal {
	char key[64];
	int val;
};

struct TokList {
	TokNode *head;
};

struct TokNode {
	enum TokType tok;
	char name[64];
	TokNode *next;
};

struct VarNode {
	char name[64];
	VarNode *next;
};

struct CompoundStatement {
	StatementList *statements;
};

struct Program {
	CompoundStatement *cs;
};

struct AST {
	Program *program;
};

struct StatementList {
	Statement *head;	
	Statement *tail;
};

struct Factor {
	enum DataType type;
	enum OpType op;
	Expression *e;
	Factor *f;
	char data[64];
};

struct Term {
	Factor *f;
	enum OpType op;
	Term *t;
};

struct Expression {
	Term *t;
	enum OpType op;
	Expression *e;
};

struct AssignmentStatement {
	char left[64];
	Expression *e;
};

struct Statement {
	enum StatementType type;
	CompoundStatement *cs;
	AssignmentStatement *as;
	Statement *next;
};

TokList *newTokList();
int addTok(TokList *l, enum TokType tok, char *name);
int deleteTokList(TokList *l);
int getVarOrInt(FILE *fp, char *dest, int size);
int scan(FILE *fp, TokList *list);
AST *parse(TokList *list);
Program *program();
CompoundStatement *compoundStatement();
StatementList *getStatements();
Statement *statement();
AssignmentStatement *assignmentStatement();
Expression *expression();
Term *term();
Factor *factor();
int consume(enum TokType type);
void deleteAST(AST *ast);
void deleteProgram(Program *program);
void deleteCompoundStatement(CompoundStatement *cs);
void deleteStatementList(StatementList *sl);
void deleteStatement(Statement *s);
void deleteAssignmentStatement(AssignmentStatement *as);
void deleteExpression(Expression *e);
void deleteTerm(Term *t);
void deleteFactor(Factor *f);
void visitCompoundStatement(CompoundStatement *cs);
void visitStatement(Statement *s);
void visitAssignmentStatement(AssignmentStatement *as);
void visitExpression(Expression *e);
void visitTerm(Term *t);
void visitFactor(Factor *f);
