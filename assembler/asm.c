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
	InsList *insList;
	Instruction *ins;
	LabelList *labelList;
	char *fnEXT;
	char fnOut[32];
	char labelNum[64];
	char bits[18];
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

	insList = newInsList();
	if(insList == NULL){
		exit(-1);
	}	
	
	labelList = newLabelList();
	if(labelList == NULL){
		exit(-1);
	}
	
	parse(fp, insList);
	fclose(fp);
		
	ic = 0;
	offset = 0;
	// Find labels and assign ROM addresses
	for(InsNode *n = insList->head; n != NULL; n=n->next, ic++){
		if(n->data->val_type == VAL_SYMBOL && n->data->ins_type == VAL_SYMBOL){
			addLabel(labelList, n->data->val, ic - offset++);
		}
	} 
	// Replace all uses of labels with their numbers
	for(InsNode *n = insList->head; n != NULL; n=n->next){
		if(n->data->ins_type == INS_A && n->data->val_type == VAL_SYMBOL){
			sprintf(labelNum, "%d", getLabelVal(labelList, n->data->val));
			//printf("%s: %s\n", labelNum, n->data->val);
			strcpy(n->data->val, labelNum);
			n->data->val_type = VAL_NUMERIC;
		}
	}

	strcpy(fnOut, argv[1]);
	fnOut[strlen(fnOut) - 3] = '\0';
	strcat(fnOut, "hack");

	bits[16] = '\n';
	bits[17] = '\0';
	fp = fopen(fnOut, "w");
	if(fp != NULL){
		for(InsNode *n = insList->head; n != NULL; n=n->next){
			if(n->data->ins_type != INS_P){
				decToBin16(buildIns(n->data), bits);
				//printf("%d, %s", buildIns(n->data), bits);
				fwrite(bits, strlen(bits), 1, fp);
			}
		}
	}
	fclose(fp);
	deleteInsList(insList);
	deleteLabelList(labelList);
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

