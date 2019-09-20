#include "astree.h"

AST* astCreate(int type, HASH_NODE* symbol, AST* s0, AST* s1, AST* s2, AST* s3){
	AST* newnode = 0;
	newnode = (AST*)calloc(1, sizeof(AST));
	newnode->type = type;
	newnode->symbol = symbol;
	newnode->son[0] = s0;
	newnode->son[1] = s1;
	newnode->son[2] = s2;
	newnode->son[3] = s3;
	return newnode;
}

void astPrint(AST* node, int level){
	if(!node)
		return;

	for(int i = 0; i < level; i++)
		fprintf(stderr, "  ");
	fprintf(stderr, "AST(");
	switch(node->type){
		case AST_SYMBOL: fprintf(stderr, "AST_SYMBOL,"); break;
		case AST_ADD: fprintf(stderr, "AST_ADD,"); break;
		case AST_SUB: fprintf(stderr, "AST_SUB,"); break;
		case AST_MUL: fprintf(stderr, "AST_MUL,"); break;
		case AST_DIV: fprintf(stderr, "AST_DIV,"); break;
		case AST_LT: fprintf(stderr, "AST_LT,"); break;
		case AST_GT: fprintf(stderr, "AST_GT,"); break;
		case AST_AND: fprintf(stderr, "AST_AND,"); break;
		case AST_OR: fprintf(stderr, "AST_OR,"); break;
		case AST_LE: fprintf(stderr, "AST_LE,"); break;
		case AST_GE: fprintf(stderr, "AST_GE,"); break;
		case AST_EQ: fprintf(stderr, "AST_EQ,"); break;
		case AST_DIF: fprintf(stderr, "AST_DIF,"); break;
		default: break;	
	}

	if(node->symbol)
		fprintf(stderr, "%s,\n", node->symbol->text);
	else
		fprintf(stderr, """,\n");
	
	for(int i = 0; i < MAX_SONS; ++i)
		astPrint(node->son[i], level + 1);
}

