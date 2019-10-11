#include "semantic.h"

int semanticError = 0;

void checkAndSetTypes(AST* node){
	if(!node)
		return;

	// take all declaration nodes
	if(node->type == AST_VARDECL || node->type == AST_VECTDECL || node->type == AST_FUNDECL){
		if(node->symbol && node->symbol->type != SYMBOL_IDENTIFIER){
			fprintf(stderr, "Semantic ERROR: Symbol %s already declared.\n", node->symbol->text);
			semanticError++;
		}

		// set the correct type (nature)
		if(node->type == AST_VARDECL)
			node->symbol->type = SYMBOL_SCALAR;
		if(node->type == AST_VECTDECL)
			node->symbol->type = SYMBOL_VECTOR;
		if(node->type == AST_FUNDECL)
			node->symbol->type = SYMBOL_FUNCTION;

		// set also the datatype from the declaration
		if(node->son[0]->type == AST_TYPEBYTE)
			node->symbol->datatype = DATATYPE_BYTE;
		if(node->son[0]->type == AST_TYPEINT)
			node->symbol->datatype = DATATYPE_INT;
		if(node->son[0]->type == AST_TYPELONG)
			node->symbol->datatype = DATATYPE_LONG;
		if(node->son[0]->type == AST_TYPEFLOAT)
			node->symbol->datatype = DATATYPE_FLOAT;
		if(node->son[0]->type == AST_TYPEBOOL)
			node->symbol->datatype = DATATYPE_BOOL;
	}

	for(int i = 0; i < MAX_SONS; ++i)
		checkAndSetTypes(node->son[i]);
}

void checkUndeclared(void){
	semanticError += hashCheckUndeclared();
}

int getSemanticErrors(void){
	return semanticError;
}
