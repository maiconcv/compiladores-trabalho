#include "hash.h"

HASH_NODE* Table[HASH_SIZE];

void hashInit(void){
	int i;
	for(i = 0; i < HASH_SIZE; i++)
		Table[i] = 0;
}

int hashAddress(char* text){
	int address = 1;
	int i;
	for(i = 0; i < strlen(text); i++)
		address = (address * text[i]) % HASH_SIZE + 1;
	return address - 1;
}

HASH_NODE* hashFind(char* text){
	int address = hashAddress(text);
	for(HASH_NODE* node = Table[address]; node; node = node->next)
		if(!strcmp(text, node->text))
			return node;
	return 0;
}

HASH_NODE* hashInsert(char* text, int type){
	HASH_NODE* newnode;
	if((newnode = hashFind(text)) != 0)
		return newnode;
	int address = hashAddress(text);
	newnode = (HASH_NODE*)calloc(1, sizeof(HASH_NODE));
	newnode->type = type;
	newnode->text = (char*)calloc(strlen(text) + 1, sizeof(char));
	strcpy(newnode->text, text);
	newnode->next = Table[address];
	Table[address] = newnode;
	return newnode;
}

void hashPrint(void){
	int i;
	HASH_NODE* node;
	for(i = 0; i < HASH_SIZE; i++)
		for(node = Table[i]; node; node = node->next)
			printf("Table[%d] has %s\n", i, node->text);
}

int hashCheckUndeclared(void){
	HASH_NODE* node;
	int errors = 0;

	for(int i = 0; i < HASH_SIZE; ++i){
		for(node = Table[i]; node; node = node->next){
			if(node->type == SYMBOL_IDENTIFIER){
				fprintf(stderr, "Semantic ERROR: Symbol %s undeclared.\n", node->text);
				errors++;
			}
		}
	}

	return errors;
}

