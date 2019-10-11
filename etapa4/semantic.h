#ifndef COMP_SEMANTIC_H
#define COMP_SEMANTIC_H

#include "hash.h"
#include "astree.h"

void checkAndSetTypes(AST* node);
void checkUndeclared(void);
int getSemanticErrors(void);

#endif // COMP_SEMANTIC_H
