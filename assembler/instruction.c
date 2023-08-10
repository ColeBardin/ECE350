#include "instruction.h"
#include <stdlib.h>
#include <string.h>

const int numComps = 30;
const int compThresh = 19;
KeyVal comps[] = {
{"0",COMP_0},
{"1",COMP_1},
{"-1",COMP_NEG_1},
{"D",COMP_D},
{"A",COMP_AM},
{"!D",COMP_NOT_D},
{"!A",COMP_NOT_AM},
{"-D",COMP_NEG_D},
{"-A",COMP_NEG_AM},
{"D+1",COMP_D_PLUS_1},
{"A+1",COMP_AM_PLUS_1},
{"D-1",COMP_D_MINUS_1},
{"A-1",COMP_AM_MINUS_1},
{"D+A",COMP_D_PLUS_AM},
{"A+D",COMP_D_PLUS_AM},
{"D-A",COMP_D_MINUS_AM},
{"A-D",COMP_AM_MINUS_D},
{"D&A",COMP_D_AND_AM},
{"D|A",COMP_D_OR_AM},
{"M",COMP_AM},
{"!M",COMP_NOT_AM},
{"-M",COMP_NEG_AM},
{"M+1",COMP_AM_PLUS_1},
{"M-1",COMP_AM_MINUS_1},
{"D+M",COMP_D_PLUS_AM},
{"M+D",COMP_D_PLUS_AM},
{"D-M",COMP_D_MINUS_AM},
{"M-D",COMP_AM_MINUS_D},
{"D&M",COMP_D_AND_AM},
{"D|M",COMP_D_OR_AM}};

KeyVal jumps[] = {
{"", JMP_NULL},
{"JGT", JMP_GT},
{"JEQ", JMP_EQ},
{"JGE", JMP_GE},
{"JLT", JMP_LT},
{"JNE", JMP_NE},
{"JLE", JMP_LE},
{"JMP", JMP_UNCON}};

KeyVal defaultVars[] = {
{"SP",0}, 
{"LCL",1}, 
{"ARG",2}, 
{"THIS",3}, 
{"THAT",4}, 
{"R0",0}, 
{"R1",1}, 
{"R2",2}, 
{"R3",3}, 
{"R4",4}, 
{"R5",5}, 
{"R6",6}, 
{"R7",7}, 
{"R8",8}, 
{"R9",9}, 
{"R10",10}, 
{"R11",11}, 
{"R12",12}, 
{"R13",13}, 
{"R14",14}, 
{"R15",15}, 
{"SCREEN", 16384}, 
{"KBD", 24576}};

void decToBin16(uint16_t val, char *buf){
	int i;
	for(i = 0; i < 16; i++){
		buf[15-i] = '0' + ((val & (1 << i)) > 0);
	}
}

uint16_t buildIns(Instruction *ins){
	uint16_t bits = 0;
	int val;

	if(ins->ins_type == INS_A){
		bits |= INS_SHIFT(ins->ins_type);
		bits |= (atoi(ins->val) & 0x7FFF);
		return bits;
	}else if(ins->ins_type == INS_C){
		bits |= INS_SHIFT(ins->ins_type);
		bits |= XX_SHIFT(3);
		bits |= A_REG_SEL_SHIFT(ins->reg);
		bits |= CMP_SHIFT(ins->comp);
		bits |= DEST_SHIFT(ins->dest);
		bits |= JMP_SHIFT(ins->jmp);
		return bits;
	}
	
	return bits;
}

void determineCompute(char *line, InsList *list){
	char *pos;
	uint8_t a;
	enum JMP_T jmp;
	enum COMP_T cmp;
	Computation comp;
	Instruction ins;

	// lines with ; char
	pos = strchr(line, ';');
	if(pos != NULL){
		strcpy(comp.jump, ++pos);
	}else{
		comp.jump[0] = '\0';
	}
	// lines with = char
	pos = strchr(line, '=');
	if(pos != NULL){
		strcpy(comp.comp, ++pos);
		strcpy(comp.dest, line);
		// Set = to be end of dest string
		for(int i = 0; i < strlen(comp.dest); i++){
			if(comp.dest[i] == '='){
				comp.dest[i] = '\0';
				break;
			}
		}	
	}else{
		strcpy(comp.comp, line);
		comp.dest[0] = '\0';
	}
	// Set ; to be end of comp string
	for(int i = 0; i < strlen(comp.comp); i++){
		if(comp.comp[i] == ';'){
			comp.comp[i] = '\0';
			break;
		}
	}
	// Build instruction based on tokenized values
	buildComp(&comp, &ins);
	// Add instruction to list
	addInstruction(list, ins.ins_type, ins.val_type, ins.val, ins.comp, ins.reg, ins.dest, ins.jmp);
}

void buildComp(Computation *cmp, Instruction *ins){
	char *pos;
	int n, i;
	
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
	for(i = 0; i < numComps; i++){
		if(!strcmp(pos, comps[i].key)){
			ins->comp = comps[i].val;
			ins->reg = (i >= compThresh) ? REG_M : REG_A;
		}
	}

	// Decode jump instruction
	pos = cmp->jump;
	n = sizeof(jumps) / sizeof(KeyVal);
	for(i = 0; i < n; i++){
		if(!strcmp(pos, jumps[i].key)){
			ins->jmp = jumps[i].val;
		}
	}	
}

InsList *newInsList(){
	InsList *list;

	list = malloc(sizeof(InsList));
	if(list == NULL){
		return NULL;
	}
	list->head = NULL;
	return list;	
}

int deleteInsList(InsList *list){
	InsNode *c, *n;
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

int addInstruction(InsList *list, enum INS_T insType, enum VAL_T valType, char *val, enum COMP_T comp, enum REG_SEL reg, enum DEST_T dest, enum JMP_T jmp){
	InsNode *node;
	InsNode *p;

	if(list == NULL){
		return -1;
	}

	// Build node
	node = malloc(sizeof(InsNode));
	if(node == NULL){
		return -1;
	}	
	node->next = NULL;
	node->data = malloc(sizeof(struct Instruction));
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
	return 1;
}

LabelList *newLabelList(){
	LabelList *list;
	list = malloc(sizeof(LabelList));
	if(list == NULL){
		return NULL;
	}
	list->head = NULL;
	return list;
}

int deleteLabelList(LabelList *list){
	LabelNode *c, *n;
	if(list == NULL){
		return -1;
	}

	if(list->head == NULL){
		free(list);
		return 0;
	}
	
	for(c = list->head; c != NULL; c = n){
		n = c->next;
		free(c);
	}
	free(list);
	return 0;
}

int addLabel(LabelList *list, char *name, int val){
	LabelNode *node;
	LabelNode *p;

	if(list == NULL){
		return -1;
	}

	// Build node
	node = malloc(sizeof(LabelNode));
	if(node == NULL){
		return -1;
	}	

	strcpy(node->name, name);
	node->val = val;
	node->next = NULL;
	
	// add to list
	if(list->head == NULL){
		list->head = node;
	}else{
		for(p = list->head; p->next != NULL; p = p->next);
		p->next = node;
	}
	return 1;
}

int getLabelVal(LabelList *list, char *target){
	LabelNode *n;

	for(n = list->head; n != NULL; n = n->next){
		if(!strcmp(target, n->name)){
			return n->val;
		}
	}
	return -1;
}

int loadDefaultVariables(LabelList *list){
	int n, i;
	n = sizeof(defaultVars) / sizeof(KeyVal);
	for(i = 0; i < n; i++){
		addLabel(list, defaultVars[i].key, defaultVars[i].val);
	}
	return 0;	
}

