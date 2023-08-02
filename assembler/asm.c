#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "instruction.h"

int parse(FILE *fp, InsList *dest);
int tokenize(char *line, InsList *list);
void determineCompute(char *line, InsList *list);

int main(int argc, char **argv){
	FILE *fp;
	InsList *list;
	Instruction *ins;
	char *fnEXT;
	int ic;
	int offset;
	
	if(argc != 2){
		exit(-1);
	}
	fnEXT = strrchr(argv[1], '.');
	if(strcmp(fnEXT, ".asm")){
		fprintf(stderr, "Incorrect file format. Must be .asm\n");
		exit(-1);
	}

	fp = fopen(argv[1], "r");
	if(fp == NULL){
		exit(-1);
	}	

	list = newInsList();
	if(list == NULL){
		exit(-1);
	}	
	
	parse(fp, list);
		
	ic = 0;
	offset = 0;
	for(InsNode *n = list->head; n != NULL; n=n->next, ic++){
		ins = n->data;
		if(ins->val_type == VAL_SYMBOL && ins->ins_type == VAL_SYMBOL){
			printf("ins: %d: %s\n", ic, ins->val);
		}
	} 

	deleteInsList(list);
	exit(0);
}

int parse(FILE *fp, InsList *dest){
	char line[512];
	char c;
	int i, lc, loc;
	
	c = 0;
	lc = 0;
	while(c != EOF){
		i = 0;
		while(1){
			c = fgetc(fp);
			if(c == ' ' || c == '\t'){
				continue;
			}else if(c == '\n' || c == EOF){
				break;
			}else{
				line[i] = c;	
				i++;	
			}
		}
	
		for(loc = 0; loc <= strlen(line); loc++){
			if(line[loc] == '/') break;
			if(loc == (strlen(line))){
				loc = 0;
				break;
			}
		}
		if(loc == 0 && line[0] == '/'){
			// entire comment line
			continue;
		}else if(loc != 0){
			// comment starting at pos loc
			line[loc] = '\0';
		}
		if(c != EOF){
			line[i] = '\0';
			tokenize(line, dest);
			//printf("Line %d: %s\n", lc, line);
			lc++;
		}
	}
	return 0;
}

int tokenize(char *line, InsList *list){
	int val;

	switch(line[0]){
		case '(':
			line[strlen(line) - 1] = '\0';
			addInstruction(list, INS_P, VAL_SYMBOL, &line[1], 0, 0, 0, 0);
			break;
		case '@':
			val = atoi(&line[1]);
			if(val == 0){
				addInstruction(list, INS_A, VAL_SYMBOL, &line[1], 0, 0, 0, 0);
			}else{
				addInstruction(list, INS_A, VAL_NUMERIC, &line[1], 0, 0, 0, 0);
			}
			break;
		default:
			determineCompute(line, list);
			break;		
	}	
	return 0;
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
	buildComp(&comp, &ins);
	addInstruction(list, ins.ins_type, ins.val_type, ins.val, ins.comp, ins.reg, ins.dest, ins.jmp);
}

