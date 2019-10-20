#ifndef COMP_SEMANTIC_H
#define COMP_SEMANTIC_H

#include "hash.h"
#include "astree.h"

#define TYPE_NUMERIC 1
#define TYPE_BOOLEAN 2
#define TYPE_ERROR 3

void checkAndSetTypes(AST* node);
void checkUndeclared(void);
int getSemanticErrors(void);
void checkOperands(AST* node);
int checkBracketsType(AST* node, int expectedType);

#endif // COMP_SEMANTIC_H
