#ifndef COMP_ASTREE_H
#define COMP_ASTREE_H

#include <stdio.h>
#include "hash.h"

#define MAX_SONS 4

#define AST_SYMBOL 1
#define AST_ADD 2
#define AST_SUB 3
#define AST_MUL 4
#define AST_DIV 5
#define AST_LT 6
#define AST_GT 7
#define AST_AND 8
#define AST_OR 9
#define AST_LE 10
#define AST_GE 11
#define AST_EQ 12
#define AST_DIF 13

typedef struct astree_node{
	int type;
	HASH_NODE* symbol;
	struct astree_node* son[MAX_SONS];
} AST;

AST* astCreate(int type, HASH_NODE* symbol, AST* s0, AST* s1, AST* s2, AST* s3);
void astPrint(AST* node, int level);

#endif // COMP_ASTREE_H
