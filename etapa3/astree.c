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
		case AST_NOT: fprintf(stderr, "AST_NOT,"); break;
		case AST_BRACKETS: fprintf(stderr, "AST_BRACKETS,"); break;
		case AST_VECTREAD: fprintf(stderr, "AST_VECTREAD,"); break;
		case AST_ASSIGN: fprintf(stderr, "AST_ASSIGN,"); break;
		case AST_LCMD: fprintf(stderr, "AST_LCMD,"); break;
		case AST_VARDECL: fprintf(stderr, "AST_VARDECL,"); break;
		case AST_FUNDECL: fprintf(stderr, "AST_FUNDECL,"); break;
		case AST_VECTDECL: fprintf(stderr, "AST_VECTDECL,"); break;
		case AST_LDECL: fprintf(stderr, "AST_LDECL,"); break;
		case AST_TYPEBYTE: fprintf(stderr, "AST_TYPEBYTE,"); break;
		case AST_TYPEINT: fprintf(stderr, "AST_TYPEINT,"); break;
		case AST_TYPELONG: fprintf(stderr, "AST_TYPELONG,"); break;
		case AST_TYPEFLOAT: fprintf(stderr, "AST_TYPEFLOAT,"); break;
		case AST_TYPEBOOL: fprintf(stderr, "AST_TYPEBOOL,"); break;
		case AST_VECTASSIGN: fprintf(stderr, "AST_VECTASSIGN,"); break;
		case AST_READ: fprintf(stderr, "AST_READ,"); break;
		case AST_RETURN: fprintf(stderr, "AST_RETURN,"); break;
		case AST_IF: fprintf(stderr, "AST_IF,"); break;
		case AST_IFELSE: fprintf(stderr, "AST_IFELSE,"); break;
		case AST_WHILE: fprintf(stderr, "AST_WHILE,"); break;
		case AST_BREAK: fprintf(stderr, "AST_BREAK,"); break;
		case AST_FOR: fprintf(stderr, "AST_FOR,"); break;
		case AST_VECTINIT: fprintf(stderr, "AST_VECTINIT,"); break;
		case AST_LSTLIT: fprintf(stderr, "AST_LSTLIT,"); break;
		case AST_LSTPARAM: fprintf(stderr, "AST_LSTPARAM,"); break;
		case AST_PARAM: fprintf(stderr, "AST_PARAM,"); break;
		case AST_PRINT: fprintf(stderr, "AST_PRINT,"); break;
		case AST_PRINTARG: fprintf(stderr, "AST_PRINTARG,"); break;
		case AST_FUNCALL: fprintf(stderr, "AST_FUNCALL,"); break;
		case AST_FUNARG: fprintf(stderr, "AST_FUNARG,"); break;
		default: break;	
	}

	if(node->symbol)
		fprintf(stderr, "%s,\n", node->symbol->text);
	else
		fprintf(stderr, """,\n");
	
	for(int i = 0; i < MAX_SONS; ++i)
		astPrint(node->son[i], level + 1);
}

