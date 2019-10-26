#include "semantic.h"

int semanticError = 0;

void checkAndSetTypes(AST* node){
	if(!node)
		return;

	// take all declaration nodes
	if(node->type == AST_VARDECL || node->type == AST_VECTDECL || node->type == AST_FUNDECL){
		if(node->symbol && node->symbol->type != SYMBOL_IDENTIFIER){
			fprintf(stderr, "Semantic ERROR: Symbol %s already declared at line %d.\n", node->symbol->text, node->line);
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
				    node->son[i]->symbol->datatype != DATATYPE_BOOL) ||

				   (node->son[i]->type == AST_BRACKETS &&
				    checkBracketsType(node->son[i], TYPE_NUMERIC) == TYPE_NUMERIC))
					;
				else{
					fprintf(stderr, "Semantic ERROR: Operands not compatible at line %d. Expecting only numeric operands.\n", node->line);
					semanticError++;
				}
			}
			break;
		case AST_LT:
		case AST_GT:
		case AST_LE:
		case AST_GE:
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
				    node->son[i]->symbol->datatype != DATATYPE_BOOL) ||

			    	   (node->son[i]->type == AST_BRACKETS &&
				    checkBracketsType(node->son[i], TYPE_NUMERIC) == TYPE_NUMERIC))
					;
				else{
					fprintf(stderr, "Semantic ERROR: Operands not compatible at line %d.\n", node->line);
					semanticError++;
				}
			}
			break;
		case AST_EQ:
		case AST_DIF:
			if((checkBracketsType(node->son[0], TYPE_NUMERIC) == TYPE_NUMERIC &&
			    checkBracketsType(node->son[1], TYPE_NUMERIC) == TYPE_NUMERIC) ||

			   (checkBracketsType(node->son[0], TYPE_BOOLEAN) == TYPE_BOOLEAN &&
			    checkBracketsType(node->son[1], TYPE_BOOLEAN) == TYPE_BOOLEAN))
				;
			else{
				fprintf(stderr, "Semantic ERROR: Operands not compatible at line %d.\n", node->line);
				semanticError++;
			}
			break;
		case AST_AND:
		case AST_OR:
		case AST_NOT:
			for(int i = 0; i < 2; i++){
				if(node->type == AST_AND || node->type == AST_OR || (node->type == AST_NOT && i == 0)){
					if(node->son[i]->type == AST_AND ||
					   node->son[i]->type == AST_OR ||
					   node->son[i]->type == AST_NOT ||

					   (node->son[i]->type == AST_SYMBOL &&
					    node->son[i]->symbol->type == SYMBOL_SCALAR &&
					    node->son[i]->symbol->datatype == DATATYPE_BOOL) ||

					   (node->son[i]->type == AST_SYMBOL &&
					    node->son[i]->symbol->type == SYMBOL_LITBOOL) ||

					   (node->son[i]->type == AST_VECTREAD &&
					    node->son[i]->symbol->datatype == DATATYPE_BOOL) ||

					   (node->son[i]->type == AST_FUNCALL &&
					    node->son[i]->symbol->datatype == DATATYPE_BOOL) ||

					   (node->son[i]->type == AST_BRACKETS &&
					    checkBracketsType(node->son[i], TYPE_BOOLEAN) == TYPE_BOOLEAN))
						;
					else{
						fprintf(stderr, "Semantic ERROR: Operands not compatible at line %d.\n", node->line);
						semanticError++;
					}
				}
			}
			break;
		case AST_ASSIGN:
			if((node->symbol->datatype != DATATYPE_BOOL &&
			    checkBracketsType(node->son[0], TYPE_NUMERIC) == TYPE_NUMERIC) ||

			   (node->symbol->datatype == DATATYPE_BOOL &&
		   	    checkBracketsType(node->son[0], TYPE_BOOLEAN) == TYPE_BOOLEAN))
				;
			else{
				fprintf(stderr, "Semantic ERROR: Operands not compatible at line %d.\n", node->line);
				semanticError++;
			}
			break;
		case AST_VECTASSIGN:
			if(!(checkBracketsType(node->son[0], TYPE_NUMERIC) == TYPE_NUMERIC))
			{
				fprintf(stderr, "Semantic ERROR: Vector index must be a numeric type at line %d.\n", node->line);
				semanticError++;
			}
			if((node->symbol->datatype != DATATYPE_BOOL &&
			    checkBracketsType(node->son[1], TYPE_NUMERIC) == TYPE_NUMERIC) ||

			   (node->symbol->datatype == DATATYPE_BOOL &&
		   	    checkBracketsType(node->son[1], TYPE_BOOLEAN) == TYPE_BOOLEAN))
				;
			else{
				fprintf(stderr, "Semantic ERROR: Operands not compatible at line %d.\n", node->line);
				semanticError++;
			}
			break;
		case AST_VECTREAD:
			if(!(checkBracketsType(node->son[0], TYPE_NUMERIC) == TYPE_NUMERIC))
			{
				fprintf(stderr, "Semantic ERROR: Vector index must be a numeric type at line %d.\n", node->line);
				semanticError++;
			}
			break;
		case AST_VARDECL:
			if((node->son[0]->type != AST_TYPEBOOL &&
			    checkBracketsType(node->son[1], TYPE_NUMERIC) == TYPE_NUMERIC) ||
		   	   (node->son[0]->type == AST_TYPEBOOL &&
			    checkBracketsType(node->son[1], TYPE_BOOLEAN) == TYPE_BOOLEAN))
			   	;
			else{
				fprintf(stderr, "Semantic ERROR: Init value for %s variable is not compatible with its type at line %d.\n", node->symbol->text, node->line);
				semanticError++;
			}
			break;
		case AST_VECTDECL:
			if((node->son[0]->type != AST_TYPEBOOL &&
			    checkBracketsType(node->son[2], TYPE_NUMERIC) == TYPE_NUMERIC) ||
			   (node->son[0]->type == AST_TYPEBOOL &&
    			    checkBracketsType(node->son[2], TYPE_BOOLEAN) == TYPE_BOOLEAN))
			    	;
			else{
				fprintf(stderr, "Semantic ERROR: Init values for %s vector are not compatible with its type at line %d.\n", node->symbol->text, node->line);
				semanticError++;
			}
			break;
		default:
			break;
	}

	for(int i = 0; i < MAX_SONS; i++)
		checkOperands(node->son[i]);
}

int checkBracketsType(AST* node, int expectedType){
	if(!node)
		return expectedType;

	switch(node->type){
		case AST_SYMBOL:
			if(node->symbol->type == SYMBOL_LITINT ||
			   node->symbol->type == SYMBOL_LITREAL ||
			   node->symbol->type == SYMBOL_LITCHAR ||
			   (node->symbol->type == SYMBOL_SCALAR &&
			    node->symbol->datatype != DATATYPE_BOOL))
				return TYPE_NUMERIC;
			else if(node->symbol->type == SYMBOL_LITBOOL ||
				(node->symbol->type == SYMBOL_SCALAR &&
				 node->symbol->datatype == DATATYPE_BOOL))
				return TYPE_BOOLEAN;
			else
				return TYPE_ERROR;
			break;
		case AST_VECTREAD:
		case AST_FUNCALL:
			if(node->symbol->datatype != DATATYPE_BOOL)
				return TYPE_NUMERIC;
			else if(node->symbol->datatype == DATATYPE_BOOL)
				return TYPE_BOOLEAN;
			else
				return TYPE_ERROR;
			break;
		case AST_ADD:
		case AST_SUB:
		case AST_MUL:
		case AST_DIV:
			if(/*expectedType == TYPE_NUMERIC &&*/
			   checkBracketsType(node->son[0], expectedType) == TYPE_NUMERIC &&
			   checkBracketsType(node->son[1], expectedType) == TYPE_NUMERIC)
				return TYPE_NUMERIC;
			else
				return TYPE_ERROR;
			break;
		case AST_AND:
		case AST_OR:
		case AST_NOT:
			if(expectedType == TYPE_BOOLEAN &&
			   checkBracketsType(node->son[0], expectedType) == TYPE_BOOLEAN &&
			   checkBracketsType(node->son[1], expectedType) == TYPE_BOOLEAN)
				return TYPE_BOOLEAN;
			else
				return TYPE_ERROR;
			break;
		case AST_LT:
		case AST_GT:
		case AST_LE:
		case AST_GE:
			if(expectedType == TYPE_BOOLEAN &&
		   	   checkBracketsType(node->son[0], expectedType) == TYPE_NUMERIC &&
		   	   checkBracketsType(node->son[1], expectedType) == TYPE_NUMERIC)
				return TYPE_BOOLEAN;
			else
				return TYPE_ERROR;
			break;
		case AST_EQ:
		case AST_DIF:
			if(expectedType == TYPE_BOOLEAN &&
		   	   checkBracketsType(node->son[0], expectedType) == TYPE_BOOLEAN &&
		   	   checkBracketsType(node->son[1], expectedType) == TYPE_BOOLEAN)
			   	return TYPE_BOOLEAN;
			else if(expectedType == TYPE_BOOLEAN &&
		   	   	checkBracketsType(node->son[0], expectedType) == TYPE_NUMERIC &&
		   	   	checkBracketsType(node->son[1], expectedType) == TYPE_NUMERIC)
				return TYPE_BOOLEAN;
			else
				return TYPE_ERROR;
			break;
		case AST_BRACKETS:
			return checkBracketsType(node->son[0], expectedType);
			break;
		case AST_VECTINIT:
			return checkBracketsType(node->son[0], expectedType);
			break;
		case AST_LSTLIT:
			if(checkBracketsType(node->son[0], expectedType) == expectedType)
				return checkBracketsType(node->son[1], expectedType);
			else
				return TYPE_ERROR;
			break;
		default:
			break;
	}
	return TYPE_ERROR;
}
