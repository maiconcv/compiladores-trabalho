#ifndef COMP_TACS_H
#define COMP_TACS_H

#include "hash.h"
#include "astree.h"
#include "litstr_to_var.h"
#include "semantic.h"

#define TAC_SYMBOL 1
#define TAC_ADD 2
#define TAC_SUB 3
#define TAC_MUL 4
#define TAC_DIV 5
#define TAC_MOVE 6
#define TAC_IFZ 7
#define TAC_LABEL 8
#define TAC_LT 9
#define TAC_GT 10
#define TAC_LE 11
#define TAC_GE 12
#define TAC_EQ 13
#define TAC_DIF 14
#define TAC_AND 15
#define TAC_OR 16
#define TAC_NOT 17
#define TAC_JUMP 18
#define TAC_READ 19
#define TAC_BEGINFUN 20
#define TAC_ENDFUN 21
#define TAC_RETURN 22
#define TAC_PRINT 23
#define TAC_MOVEVECT 24
#define TAC_VECTREAD 25
#define TAC_PARAM 26
#define TAC_VARDECL 27
#define TAC_VECTDECL 28
#define TAC_FUNCALL 29
#define TAC_ARG 30
#define TAC_BREAK 31

#define LABEL_LOGIC_OP "LB"

typedef struct tac_node {
	int type;
	HASH_NODE* res;
	HASH_NODE* op1;
	HASH_NODE* op2;
	struct tac_node* prev;
	struct tac_node* next;
} TAC;

TAC* tacCreate(int type, HASH_NODE* res, HASH_NODE* op1, HASH_NODE* op2);
void tacPrintSingle(TAC* tac);
void tacPrintBackwards(TAC* tac);
TAC* tacJoin(TAC* l1, TAC* l2);
TAC* generateCode(AST* ast, HASH_NODE* funCallName, int funArgCounter, HASH_NODE* labelLeaveFlowControlCmd);
void tacPrintForwards(TAC* tac);
void generateASM(TAC* tac, FILE* fout);

#endif // COMP_TACS_H
