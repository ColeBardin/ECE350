#pragma once

#include <stdint.h>

#define INS_SHIFT(x) (x<<15)
#define A_REG_SEL_SHIFT(x) (x<<12)
#define CMP_SHIFT(x) (x<<6)
#define DEST_SHIFT(x) (x<<3)
#define JMP_SHIFT(x) (x)

enum INS_TYPE {
	INS_A,
	INS_C,
	INS_P,
};

enum VAL_TYPE {
	VAL_NULL,
	VAL_NUMERIC,
	VAL_SYMBOL,
};

enum COMP_TYPE {
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

enum A_REG_SEL {
	A_REG_A,
	A_REG_M,
};

enum DEST_TYPE {
	DEST_NULL,
	DEST_M,
	DEST_D,
	DEST_MD,
	DEST_A,
	DEST_AM,
	DEST_AD,
	DEST_ADM,
};

enum JMP_TYPE {
	JMP_NULL,
	JMP_GT,
	JMP_EQ,
	JMP_GE,	
	JMP_LT,	
	JMP_NE,	
	JMP_LE,	
	JMP_UNCON
};

struct instruction {
	enum INS_TYPE ins_type;
	enum VAL_TYPE val_type;
	char val[64];
	enum COMP_TYPE comp;
	enum A_REG_SEL reg;
	enum DEST_TYPE dest;
	enum JMP_TYPE jmp;
};

struct computation {
	char dest[128];
	char comp[128];
	char jump[128];
};

uint16_t build_ins(struct instruction *ins);
void build_comp(struct computation *cmp, struct instruction *ins);
