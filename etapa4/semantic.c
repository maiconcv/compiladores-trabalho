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

void checkOperands(AST* node){
	if(!node)
		return;

	switch(node->type){
		case AST_ADD:
		case AST_SUB:
		case AST_MUL:
		case AST_DIV: // check correctness of the two operands
			for(int i = 0; i < 2; i++){
				if(node->son[i]->type == AST_ADD ||
				   node->son[i]->type == AST_SUB ||
				   node->son[i]->type == AST_MUL ||
				   node->son[i]->type == AST_DIV ||
				   (node->son[i]->type == AST_SYMBOL &&
				    node->son[i]->symbol->type == SYMBOL_SCALAR &&
				    node->son[i]->symbol->datatype != DATATYPE_BOOL) ||
				   (node->son[i]->type == AST_SYMBOL &&
				    (node->son[i]->symbol->type == SYMBOL_LITINT ||
				     node->son[i]->symbol->type == SYMBOL_LITREAL)) ||
				   (node->son[i]->type == AST_VECTREAD &&
				    node->son[i]->symbol->datatype != DATATYPE_BOOL) ||
				   (node->son[i]->type == AST_FUNCALL &&
				    node->son[i]->symbol->datatype != DATATYPE_BOOL))
					;
				else{
					fprintf(stderr, "Semantic ERROR: Operands not compatible.\n");
					semanticError++;
				}
			}
			break;
		default:
			break;
	}

	for(int i = 0; i < MAX_SONS; i++)
		checkOperands(node->son[i]);
}