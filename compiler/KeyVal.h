#ifndef _KEYVAL_H_
#define _KEYVAL_H_

typedef struct KeyVal KeyVal;

struct KeyVal {
	char key[64];
	int val;
};

#endif

