#pragma once

#include <stdint.h>

typedef struct Expr Expr;
typedef struct ExprList ExprList;
typedef struct KeyVal KeyVal;

enum CmdType {
	CMD_PUSH,
	CMD_POP,
	CMD_SET,
	CMD_END,
	CMD_ADD,
	CMD_SUB,
	CMD_NEG,
	CMD_EQ,
	CMD_GT,
	CMD_LT,
	CMD_AND,
	CMD_OR,
	CMD_NOT,
	CMD_FUNC,
	CMD_CALL,
	CMD_RET,
	CMD_IF,
	CMD_GOTO,
	CMD_LBL
};

enum SegType {
	SEG_SP = 0,
	SEG_LCL,
	SEG_ARG,
	SEG_THIS,
	SEG_THAT,
	SEG_TEMP,
	SEG_STAT = 16,
	SEG_PTR,
	SEG_CONST,
	SEG_NULL,
};

struct Expr {
	Expr *next;
	enum CmdType cmd;	
	enum SegType seg;	
	int val;	
	char name[64];
};

struct ExprList {
	Expr *head;
};

struct KeyVal {
	char key[64];
	int val;
};

void writeExprs(FILE *fp, ExprList *l);
void procToks(int tokc, char **toks, ExprList *l);
void parse(ExprList *l, FILE *fp);
int tokenize(char *s, char *toks[], int max);
ExprList *newExprList();
int deleteExprList(ExprList *l);
int addExpr(ExprList *l, enum CmdType cmd, enum SegType seg, int val, char *name);
