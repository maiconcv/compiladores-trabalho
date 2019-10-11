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
#define AST_NOT 14
#define AST_BRACKETS 15
#define AST_VECTREAD 16
#define AST_ASSIGN 17
#define AST_LCMD 18
#define AST_VARDECL 19
#define AST_FUNDECL 20
#define AST_VECTDECL 21
#define AST_LDECL 22
#define AST_TYPEBYTE 23
#define AST_TYPEINT 24
#define AST_TYPELONG 25
#define AST_TYPEFLOAT 26
#define AST_TYPEBOOL 27
#define AST_VECTASSIGN 28
#define AST_READ 29
#define AST_RETURN 30
#define AST_IF 31
#define AST_IFELSE 32
#define AST_WHILE 33
#define AST_BREAK 34
#define AST_FOR 35
#define AST_VECTINIT 36
#define AST_LSTLIT 37
#define AST_LSTPARAM 38
#define AST_PARAM 39
#define AST_PRINT 40
#define AST_PRINTARG 41
#define AST_FUNCALL 42
#define AST_FUNARG 43
#define AST_BLOCK 44

typedef struct astree_node{
	int type;
	HASH_NODE* symbol;
	struct astree_node* son[MAX_SONS];
} AST;

AST* astCreate(int type, HASH_NODE* symbol, AST* s0, AST* s1, AST* s2, AST* s3);
void astPrint(AST* node, int level);
void astToSourceCode(FILE* file, AST* node, int firstParamOrArg);

#endif // COMP_ASTREE_H
