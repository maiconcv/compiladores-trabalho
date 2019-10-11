#ifndef COMP_HASH_H
#define COMP_HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 997

#define SYMBOL_IDENTIFIER 1
#define SYMBOL_LITINT 2
#define SYMBOL_LITREAL 3
#define SYMBOL_LITTRUE 4
#define SYMBOL_LITFALSE 5
#define SYMBOL_LITCHAR 6
#define SYMBOL_LITSTRING 7

typedef struct hash_node {
	int type;
	char* text;
	struct hash_node* next;
} HASH_NODE;

void hashInit(void);
int hashAddress(char* text);
HASH_NODE* hashFind(char* text);
HASH_NODE* hashInsert(char* text, int type);
void hashPrint(void);

#endif // COMP_HASH_H
