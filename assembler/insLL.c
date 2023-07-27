#include <stdlib.h>
#include "insLL.h"


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

int addInstruction(List *list, enum INS_TYPE insType, enum VAL_TYPE valType, uint16_t val, enum COMP_TYPE comp, enum A_REG_SEL reg, enum DEST_TYPE dest, enum JMP_TYPE jmp){
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
	node->data->val = val & 0x7FFF;
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

