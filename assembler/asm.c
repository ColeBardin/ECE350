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
	char *fnEXT;
	char fnOut[64];
	char labelNum[64];
	char bits[18];
	int ic;
	int offset;
	int memAddr;
	InsList *insList;
	Instruction *ins;
	LabelList *labelList;
	LabelList *varList;
	
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
		fprintf(stderr, "Error: Failed to open file %s\n", argv[1]);
		exit(-1);
	}	

	insList = newInsList();
	if(insList == NULL){
		fprintf(stderr, "Error: Failed to allocate memory for instruction list\n");
		fclose(fp);
		exit(-1);
	}	
	
	labelList = newLabelList();
	if(labelList == NULL){
		fprintf(stderr, "Error: Failed to allocate memory for lable list\n");
		fclose(fp);
		deleteInsList(insList);
		exit(-1);
	}
	varList = newLabelList();
	if(varList == NULL){
		fprintf(stderr, "Error: Failed to allocate memory for lable list\n");
		fclose(fp);
		deleteInsList(insList);
		deleteLabelList(labelList);
		exit(-1);
	}
	
	parse(fp, insList);
	fclose(fp);
		
	ic = 0;
	offset = 0;
	// Find labels and assign ROM addresses
	for(InsNode *n = insList->head; n != NULL; n=n->next, ic++){
		if(n->data->val_type == VAL_SYMBOL && n->data->ins_type == INS_P){
			addLabel(labelList, n->data->val, ic - offset++);
		}
	} 
	memAddr = 16;
	loadDefaultVariables(varList);
	// Assign all variables
	for(InsNode *n = insList->head; n != NULL; n=n->next){
		if(n->data->val_type == VAL_SYMBOL && n->data->ins_type == INS_A){
			// Symbol is not a label
			if(getLabelVal(labelList, n->data->val) == -1){
				// Not default variable
				if(getLabelVal(varList, n->data->val) == -1){
					addLabel(varList, n->data->val, memAddr++);	
				}
			}	
		}
	}
	// Replace all uses of labels with their values
	for(InsNode *n = insList->head; n != NULL; n=n->next){
		if(n->data->ins_type == INS_A && n->data->val_type == VAL_SYMBOL){
			ic = getLabelVal(labelList, n->data->val);
			if(ic == -1){
				ic = getLabelVal(varList, n->data->val);
			}
			//printf("replaced %s with %d\n", n->data->val, ic);
			snprintf(n->data->val, 64, "%d", ic);
			//strcpy(n->data->val, labelNum);
			n->data->val_type = VAL_NUMERIC;
		}
	}

	strcpy(fnOut, argv[1]);
	fnOut[strlen(fnOut) - 3] = '\0';
	strcat(fnOut, "hack");

	bits[16] = '\n';
	bits[17] = '\0';
	fp = fopen(fnOut, "w");
	if(fp == NULL){
		fprintf(stderr, "Error: failed to create or open output file %s\n", fnOut);
		deleteInsList(insList);
		deleteLabelList(labelList);
		deleteLabelList(varList);
		exit(1);
	}
	// Write all convert instructions to numbers and write binary to file	
	for(InsNode *n = insList->head; n != NULL; n=n->next){
		if(n->data->ins_type != INS_P){
			decToBin16(buildIns(n->data), bits);
			fwrite(bits, strlen(bits), 1, fp);
		}
	}
	fclose(fp);
	deleteInsList(insList);
	deleteLabelList(labelList);
	deleteLabelList(varList);
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
			}else if(c > ' ' && c < 127){
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
		case ' ':
		case '\t':
		case '\n':
		case '\0':
			break;
		case '(':
			// Label definition to be resolved later
			line[strlen(line) - 1] = '\0';
			addInstruction(list, INS_P, VAL_SYMBOL, &line[1], 0, 0, 0, 0);
			break;
		case '@':
			// Load instruction, mark labels as symbolic
			val = atoi(&line[1]);
			if(val == 0 && line[1] != '0'){
				addInstruction(list, INS_A, VAL_SYMBOL, &line[1], 0, 0, 0, 0);
			}else{
				addInstruction(list, INS_A, VAL_NUMERIC, &line[1], 0, 0, 0, 0);
			}
			break;
		default:
			// Otherwise, determine C type instruction attributes
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
	// Build instruction based on tokenized values
	buildComp(&comp, &ins);
	// Add instruction to list
	addInstruction(list, ins.ins_type, ins.val_type, ins.val, ins.comp, ins.reg, ins.dest, ins.jmp);
}

