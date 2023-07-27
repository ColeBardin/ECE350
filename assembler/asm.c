#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "instruction.h"
#include "insLL.h"

int main(int argc, char **argv){
	List *list;
	Node *n;
	
	list = newList();
	if(list == NULL){
		exit(-1);
	}	
	
	for(uint16_t i = 0; i < 10; i++){
		addInstruction(list, INS_A, 0, i, 0, 0, 0, 0);
	}
	
	for(n = list->head; n != NULL; n=n->next){
		printf("iterm with value: %d\n", n->data->val);
	}
	deleteList(list);
	exit(0);
}
