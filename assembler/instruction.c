#include <instruction.h>

uint16_t build_ins(struct instruction *ins){
	uint16_t bits = 0;

	if(ins->ins_type == INS_A){
		bits = INS_SHIFT(ins->ins_type);
		bits |= (ins->val & 0x7FFF);
		return bits;
	}else if(ins->ins_type == INS_C){
		bits = INS_SHIFT(ins->ins_type);
		bits |= A_REG_SEL_SHIFT(ins->reg);
		bits |= CMP_SHIFT(ins->comp);
		bits |= DEST_SHIFT(ins->dest);
		bits |= JMP_SHIFT(ins->jmp);
		return bits;
	}
	
	return bits;
}
