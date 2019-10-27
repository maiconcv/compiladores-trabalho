#ifndef COMP_SEMANTIC_H
#define COMP_SEMANTIC_H

#include "hash.h"
#include "astree.h"
#include <string.h>

#define TYPE_NUMERIC 1
#define TYPE_BOOLEAN 2
#define TYPE_ERROR 3

#define FUNC_ARGTYPEERROR 1
#define FUNC_ARGNUMERROR 2
#define FUNC_NOERROR 3

void checkAndSetTypes(AST* node);
void checkUndeclared(void);
int getSemanticErrors(void);
void checkOperands(AST* node);
int checkBracketsType(AST* node, int expectedType);
AST* findFunctionDeclaration(AST* node, char* functionName);
int compareFuncParamWithArg(AST* lstArgNode, AST* lstParamNode, int counter);

#endif // COMP_SEMANTIC_H
