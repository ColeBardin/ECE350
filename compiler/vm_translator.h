#pragma once

#include <stdint.h>

typedef struct Expr Expr;
typedef struct ExprList ExprList;

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
	CMD_NOT
};

enum SegType {
	SEG_SP = 0,
	SEG_LCL,
	SEG_ARG,
	SEG_POINTER,
	SEG_THIS = 3,
	SEG_THAT,
	SEG_TEMP,
	SEG_STATIC = 16,
	SEG_CONST,
	SEG_NULL,
};

struct Expr {
	Expr *next;
	enum CmdType cmd;	
	enum SegType seg;	
	uint16_t val;	
};

struct ExprList {
	Expr *head;
};

void parse(ExprList *l, FILE *fp);
int tokenize(char *s, char *toks[], int max);
ExprList *newExprList();
int deleteExprList(ExprList *l);
int addExpr(ExprList *l, enum CmdType cmd, enum SegType seg, uint16_t val);
