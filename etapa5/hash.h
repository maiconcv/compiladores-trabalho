#ifndef COMP_HASH_H
#define COMP_HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 997

#define SYMBOL_IDENTIFIER 1
#define SYMBOL_LITINT 2
#define SYMBOL_LITREAL 3
#define SYMBOL_LITBOOL 4
#define SYMBOL_LITCHAR 5
#define SYMBOL_LITSTRING 6

#define SYMBOL_SCALAR 7
#define SYMBOL_VECTOR 8
#define SYMBOL_FUNCTION 9
#define SYMBOL_LABEL 10

#define DATATYPE_BYTE 1
#define DATATYPE_INT 2
#define DATATYPE_LONG 3
#define DATATYPE_FLOAT 4
#define DATATYPE_BOOL 5

typedef struct hash_node {
	int type;
	int datatype;
	int line;
	char* text;
	struct hash_node* next;
} HASH_NODE;

void hashInit(void);
int hashAddress(char* text);
HASH_NODE* hashFind(char* text);
HASH_NODE* hashInsert(char* text, int type, int line);
void hashPrint(void);
int hashCheckUndeclared(void);
HASH_NODE* makeTemp(void);
HASH_NODE* makeLabel(void);

#endif // COMP_HASH_H
