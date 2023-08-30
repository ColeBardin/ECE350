#pragma once

#include <stdint.h>

typedef struct KeyVal KeyVal;
typedef struct TokList TokList;
typedef struct TokNode TokNode;
typedef struct VarNode VarNode;

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

TokList *newTokList();
int addTok(TokList *l, enum TokType tok, char *name);
int deleteTokList(TokList *l);
int scan(FILE *fp, TokList *list);
int getVarOrInt(FILE *fp, char *dest, int size);

