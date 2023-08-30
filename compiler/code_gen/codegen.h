#pragma once

#include <stdint.h>

typedef struct KeyVal KeyVal;
typedef struct TokList TokList;
typedef struct TokNode TokNode;
typedef struct VarNode VarNode;
typedef struct CompoundStatement CompoundStatement;
typedef struct StatementList StatementList;
typedef struct Statement Statement;
typedef struct AssignmentStatement AssignmentStatement;

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

struct StatementList {
	Statement *head;	
};

struct Statement {
	enum StatementType type;
	CompoundStatement cs;
	AssignmentStatement as;
	Statement *next;
};

struct AssignmentStatement {
	int l;
};

TokList *newTokList();
int addTok(TokList *l, enum TokType tok, char *name);
int deleteTokList(TokList *l);
int getVarOrInt(FILE *fp, char *dest, int size);
int scan(FILE *fp, TokList *list);
int parse(TokList *list);
int consume(enum TokType type);

