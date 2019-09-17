#ifndef COMP_ASTREE_H
#define COMP_ASTREE_H

#include <stdio.h>
#include "hash.h"

#define MAX_SONS 4

#define AST_SYMBOL 1
#define AST_ADD 2

typedef struct astree_node{
	int type;
	HASH_NODE* symbol;
	struct astree_node* son[MAX_SONS];
} AST;

AST* astreeCreate(int type, HASH_NODE* symbol, AST* s0, AST* s1, AST* s2, AST* s3);
void astreePrint(AST* node, int level);

#endif // COMP_ASTREE_H
