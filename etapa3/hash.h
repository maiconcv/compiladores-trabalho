#ifndef COMP_HASH_H
#define COMP_HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 997

typedef struct hash_node {
	int type;
	char* text;
	struct hash_node* next;
} HASH_NODE;

void hashInit(void);
int hashAddress(char* text);
HASH_NODE* hashFind(char* text);
HASH_NODE* hashInsert(char* text);
void hashPrint(void);

#endif // COMP_HASH_H
