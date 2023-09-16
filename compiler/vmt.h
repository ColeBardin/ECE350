#pragma once

#include <stdint.h>

typedef struct VmExpr VmExpr;
typedef struct VmExprList VmExprList;
typedef struct KeyVal KeyVal;

enum CmdType {
	CMD_PUSH = 0,
	CMD_POP,
	CMD_SET,
	CMD_END,
	CMD_ADD,
	CMD_SUB,
	CMD_MULT,
	CMD_DIV,
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

struct VmExpr {
	enum CmdType cmd;	
	enum SegType seg;	
	int val;	
	char name[64];
	VmExpr *next;
};

struct VmExprList {
	VmExpr *head;
};

struct KeyVal {
	char key[64];
	int val;
};

void writeVmExprs(FILE *fp, VmExprList *l);
void processVmToks(int tokc, char **toks, VmExprList *l);
void parseVmCode(VmExprList *l, FILE *fp);
int tokenizeVmLine(char *s, char *toks[], char *delim, int max);
VmExprList *newVmExprList();
int deleteVmExprList(VmExprList *l);
int addVmExpr(VmExprList *l, enum CmdType cmd, enum SegType seg, int val, char *name);
void translateVmFile(char *dir, char *file);
void assembleFinal(char *fno, char *dir, char files[64][64], int nfiles);
int fileAppend(FILE *dest, char *source_fn);
