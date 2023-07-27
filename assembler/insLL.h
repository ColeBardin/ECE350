#include "instruction.h"

typedef struct List List;
typedef struct Node Node;

struct List {
	Node *head;
	Node *tail;
};

struct Node {
	struct instruction *data;
	Node *next;
};

List *newList();
int deleteList(List *list);
int addInstruction(List *list, enum INS_TYPE insType, enum VAL_TYPE valType, uint16_t val, enum COMP_TYPE comp, enum A_REG_SEL reg, enum DEST_TYPE dest, enum JMP_TYPE jmp);
