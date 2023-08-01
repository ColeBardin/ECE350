#include "instruction.h"
#include <stdlib.h>
#include <string.h>

const int Ncomps = 28;
const int Nthresh = 17;
const char *comp_strings[] = {"0", "1","-1","D","A","!D","!A","-D","-A","D+1","A+1","D-1","A-1","D+A","D-A","A-D","D&A","D|A","M","!M","-M","M+1","M-1","D+M","D-M","M-D","D&M","D|M"};
const enum COMP_TYPE comp_bits[] = {COMP_0,COMP_1,COMP_NEG_1,COMP_D,COMP_AM,COMP_NOT_D,COMP_NOT_AM,COMP_NEG_D,COMP_NEG_AM,COMP_D_PLUS_1,COMP_AM_PLUS_1,COMP_D_MINUS_1,COMP_AM_MINUS_1,COMP_D_PLUS_AM,COMP_D_MINUS_AM,COMP_AM_MINUS_D,COMP_D_AND_AM,COMP_D_OR_AM,COMP_AM,COMP_NOT_AM,COMP_NEG_AM,COMP_AM_PLUS_1,COMP_AM_MINUS_1,COMP_D_PLUS_AM,COMP_D_MINUS_AM,COMP_AM_MINUS_D,COMP_D_AND_AM,COMP_D_OR_AM};

uint16_t build_ins(struct instruction *ins){
	uint16_t bits = 0;
	int val;

	if(ins->ins_type == INS_A){
		bits = INS_SHIFT(ins->ins_type);
		bits |= (atoi(ins->val) & 0x7FFF);
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

void build_comp(struct computation *cmp, struct instruction *ins){
	char *pos;
	
	ins->ins_type = INS_C;
	ins->val_type = VAL_NULL;
	ins->val[0] = '\0';

	// Set bits in dest field for each register present
	pos = cmp->dest;
	ins->dest = DEST_NULL;
	for(int i = 0; i < strlen(pos); i++){
		if(pos[i] == 'M'){
			ins->dest |= 1;
		}else if(pos[i] == 'D'){
			ins->dest |= 2;
		}else if(pos[i] == 'A'){
			ins->dest |= 4;
		}
	}
	
	// Find matching byte code for computation
	pos = cmp->comp;
	for(int i = 0; i < Ncomps; i++){
		if(!strcmp(pos, comp_strings[i])){
			ins->comp = comp_bits[i];
			ins->reg = (i > Nthresh) ? A_REG_M : A_REG_A;
		}
	}

	// Decode jump instruction
	pos = cmp->jump;
	if(pos[0] == '\0'){
		ins->jmp = JMP_NULL;
	}else if(!strncmp(pos, "JGT", 3)){
		ins->jmp = JMP_GT;
	}else if(!strncmp(pos, "JEQ", 3)){
		ins->jmp = JMP_EQ;
	}else if(!strncmp(pos, "JGE", 3)){
		ins->jmp = JMP_GE;
	}else if(!strncmp(pos, "JLT", 3)){
		ins->jmp = JMP_LT;
	}else if(!strncmp(pos, "JNE", 3)){
		ins->jmp = JMP_NE;
	}else if(!strncmp(pos, "JLE", 3)){
		ins->jmp = JMP_LE;
	}else if(!strncmp(pos, "JMP", 3)){
		ins->jmp = JMP_UNCON;
	}
}

List *newList(){
	List *list;

	list = malloc(sizeof(List));
	if(list == NULL){
		return NULL;
	}
	list->head = NULL;
	list->tail = NULL;
	return list;	
}

int deleteList(List *list){
	Node *c, *n;
	if(list == NULL){
		return -1;
	}
	if(list->head == NULL){
		free(list);
		return 0;
	}
	
	for(c = list->head; c != NULL; c = n){
		n = c->next;
		free(c->data);
		free(c);
	}
	free(list);
	return 0;
}

int addInstruction(List *list, enum INS_TYPE insType, enum VAL_TYPE valType, char *val, enum COMP_TYPE comp, enum A_REG_SEL reg, enum DEST_TYPE dest, enum JMP_TYPE jmp){
	Node *node;
	Node *p;

	if(list == NULL){
		return -1;
	}

	// Build node
	node = malloc(sizeof(Node));
	if(node == NULL){
		return -1;
	}	
	node->next = NULL;
	node->data = malloc(sizeof(struct instruction));
	if(node->data == NULL){
		free(node);
		return -1;
	}
	node->data->ins_type = insType;
	node->data->val_type = valType;
	strncpy(node->data->val, val, 64);
	node->data->comp = comp;
	node->data->reg = reg;
	node->data->dest = dest;
	node->data->jmp = jmp;
	
	// add to list
	if(list->head == NULL){
		list->head = node;
	}else{
		for(p = list->head; p->next != NULL; p = p->next);
		p->next = node;
	}
	list->tail = node;
	return 1;
}

