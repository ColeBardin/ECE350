#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"

int parse(FILE *fp, InsList *dest);
int tokenize(char *line, InsList *list);

int main(int argc, char **argv){
	FILE *fp;
	char *fnEXT;
	char fnOut[64];
	char bits[18];
	int ic, offset, memAddr;
	InsList *insList;
	LabelList *labelList;
	LabelList *varList;
	
	// Program must be passed argument
	if(argc != 2){
		exit(1);
	}
	fnEXT = strrchr(argv[1], '.');
	if(strncmp(fnEXT, ".asm", strlen(fnEXT))){
		fprintf(stderr, "Incorrect file format. Must be .asm\n");
		exit(1);
	}
	// Open File
	fp = fopen(argv[1], "r");
	if(fp == NULL){
		fprintf(stderr, "Error: Failed to open file %s\n", argv[1]);
		exit(1);
	}	
	// Create list of instructions
	insList = newInsList();
	if(insList == NULL){
		fprintf(stderr, "Error: Failed to allocate memory for instruction list\n");
		fclose(fp);
		exit(1);
	}	
	// Create list of labels	
	labelList = newLabelList();
	if(labelList == NULL){
		fprintf(stderr, "Error: Failed to allocate memory for lable list\n");
		fclose(fp);
		deleteInsList(insList);
		exit(1);
	}
	// Create list of variables
	varList = newLabelList();
	if(varList == NULL){
		fprintf(stderr, "Error: Failed to allocate memory for lable list\n");
		fclose(fp);
		deleteInsList(insList);
		deleteLabelList(labelList);
		exit(1);
	}
	
	// Process contents of file
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
			// If symbol is not a label, it's a variable
			if(getLabelVal(labelList, n->data->val) == -1){
				// If variable is not already assigned
				if(getLabelVal(varList, n->data->val) == -1){
					addLabel(varList, n->data->val, memAddr++);	
				}
			}	
		}
	}
	// Replace all labels and variables with values
	for(InsNode *n = insList->head; n != NULL; n=n->next){
		if(n->data->ins_type == INS_A && n->data->val_type == VAL_SYMBOL){
			// Get value from label list 
			ic = getLabelVal(labelList, n->data->val);
			// If symbol is not a label, get variable address
			if(ic == -1){
				ic = getLabelVal(varList, n->data->val);
			}
			snprintf(n->data->val, 64, "%d", ic);
			n->data->val_type = VAL_NUMERIC;
		}
	}

	// Create output filename
	strcpy(fnOut, argv[1]);
	fnOut[strlen(fnOut) - 3] = '\0';
	strcat(fnOut, "hack");

	bits[16] = '\n';
	bits[17] = '\0';
	fp = fopen(fnOut, "w");
	if(fp != NULL){
		// Convert instructions to binary and write to file
		for(InsNode *n = insList->head; n != NULL; n=n->next){
			if(n->data->ins_type != INS_P){
				decToBin16(buildIns(n->data), bits);
				fwrite(bits, strlen(bits), 1, fp);
			}
		}
		fclose(fp);
	}else{
		fprintf(stderr, "Error: failed to open output file %s\n", fnOut);
	}
	deleteInsList(insList);
	deleteLabelList(labelList);
	deleteLabelList(varList);
	exit(0);
}

int parse(FILE *fp, InsList *dest){
	char line[512];
	char c;
	char *p;
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
	
		// Get char index of comment marker
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
		// If not at end of file, tokenize line
		if(c != EOF){
			line[i] = '\0';
			tokenize(line, dest);
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
			// Label marker
			line[strlen(line) - 1] = '\0';
			addInstruction(list, INS_P, VAL_SYMBOL, &line[1], 0, 0, 0, 0);
			break;
		case '@':
			// Load instruction
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

