#pragma once

#include <stdint.h>

#define INS_SHIFT(x) (x<<15)
#define XX_SHIFT(x) (x<<13)
#define A_REG_SEL_SHIFT(x) (x<<12)
#define CMP_SHIFT(x) (x<<6)
#define DEST_SHIFT(x) (x<<3)
#define JMP_SHIFT(x) (x)

typedef struct InsList InsList;
typedef struct InsNode InsNode;
typedef struct LabelList LabelList;
typedef struct LabelNode LabelNode;
typedef struct Instruction Instruction;
typedef struct Computation Computation;

enum INS_T {
	INS_A,
	INS_C,
	INS_P,
};

enum VAL_T {
	VAL_NULL,
	VAL_NUMERIC,
	VAL_SYMBOL,
};

enum COMP_T {
	COMP_D_AND_AM,
	COMP_D_PLUS_AM = 0b000010,
	COMP_AM_MINUS_D = 0b000111,
	COMP_D = 0b001100,
	COMP_NOT_D,
	COMP_D_MINUS_1,
	COMP_NEG_D,
	COMP_D_MINUS_AM = 0b010011,
	COMP_D_OR_AM = 0b010101,
	COMP_D_PLUS_1 = 0b011111,
	COMP_0 = 0b101010,
	COMP_AM = 0b110000,
	COMP_NOT_AM,
	COMP_AM_MINUS_1,
	COMP_NEG_AM = 0b110011,
	COMP_AM_PLUS_1 = 0b110111,
	COMP_NEG_1 = 0b111010,
	COMP_1 = 0b111111,
};

enum REG_SEL {
	REG_A,
	REG_M,
};

enum DEST_T {
	DEST_NULL,
	DEST_M,
	DEST_D,
	DEST_MD,
	DEST_A,
	DEST_AM,
	DEST_AD,
	DEST_ADM,
};

enum JMP_T {
	JMP_NULL,
	JMP_GT,
	JMP_EQ,
	JMP_GE,	
	JMP_LT,	
	JMP_NE,	
	JMP_LE,	
	JMP_UNCON
};

struct InsList {
	InsNode *head;
};

struct InsNode {
	Instruction *data;
	InsNode *next;
};

struct LabelList {
	LabelNode *head;
};

struct LabelNode {
	char name[64];
	int val;
	LabelNode *next;
};

struct Instruction {
	enum INS_T ins_type;
	enum VAL_T val_type;
	char val[64];
	enum COMP_T comp;
	enum REG_SEL reg;
	enum DEST_T dest;
	enum JMP_T jmp;
};

struct Computation {
	char dest[128];
	char comp[128];
	char jump[128];
};


void decToBin16(uint16_t val, char *buf);
uint16_t buildIns(Instruction *ins);
void buildComp(Computation *cmp, Instruction *ins);

InsList *newInsList();
int deleteInsList(InsList *list);
int addInstruction(InsList *list, enum INS_T insType, enum VAL_T valType, char *val, enum COMP_T comp, enum REG_SEL reg, enum DEST_T dest, enum JMP_T jmp);

LabelList *newLabelList();
int deleteLabelList(LabelList *list);
int addLabel(LabelList *list, char *name, int val);
int getLabelVal(LabelList *list, char *target);
int loadDefaultVariables(LabelList *list);

