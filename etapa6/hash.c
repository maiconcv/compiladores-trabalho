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

HASH_NODE* hashInsert(char* text, int type, int line){
	HASH_NODE* newnode;
	if((newnode = hashFind(text)) != 0)
		return newnode;
	int address = hashAddress(text);
	newnode = (HASH_NODE*)calloc(1, sizeof(HASH_NODE));
	newnode->type = type;
	newnode->line = line;
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
				fprintf(stderr, "Semantic ERROR: Symbol %s undeclared at line %d.\n", node->text, node->line);
				errors++;
			}
		}
	}

	return errors;
}

HASH_NODE* makeTemp(void){
	static int serialNumber = 0;
	static char name[100];

	sprintf(name, "Temmp%d", serialNumber++);
	return hashInsert(name, SYMBOL_TEMP, 0);
}

HASH_NODE* makeLabel(void){
	static int serialNumber = 0;
	static char name[100];

	sprintf(name, "Lable%d", serialNumber++);
	return hashInsert(name, SYMBOL_LABEL, 0);
}

void generateASMGlobalVariablesFromLitValues(FILE* fout){
        static int stringCounter = 0;

	fprintf(fout, "\t.section\t.rodata\n"
			"LC0:\t.string \"%%d\"\n"
			"TRUE:\t.string \"TRUE\"\n"
			"FALSE:\t.string \"FALSE\"\n");

        for(int i = 0; i < HASH_SIZE; i++){
                for(HASH_NODE* node = Table[i]; node; node = node->next){
                        if(node){
                                if(node->type == SYMBOL_LITSTRING){
                                        addMatch(node->text, stringCounter);
                                        fprintf(fout, "\t.section\t.rodata\n"
							"_%s%d:\t.string\t%s\n", LITSTR_VAR_NAME, stringCounter, node->text);
                                        stringCounter++;
                                }
                                else if(node->type == SYMBOL_LITINT){
                                        fprintf(fout, "\t.section\t.rodata\n"
							"_%s:\t.long\t%s\n", node->text, node->text);
                                }
                                else if(node->type == SYMBOL_LITCHAR){
					addMatch(node->text, stringCounter);
                                        fprintf(fout, "\t.section\t.rodata\n"
							"_%s%d:\t.long\t%d\n", LITCHAR_VAR_NAME, stringCounter, (int)(node->text[1]));
					stringCounter++;
                                }
				else if(node->type == SYMBOL_TEMP){
					fprintf(fout, "\t.data\n"
							"_%s:\t.long\t 0\n", node->text);
				}
                        }
                }
        }
}
