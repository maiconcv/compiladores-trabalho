int main(){
	int tok;
	hashInit();
	while(running){
		tok = yylex();
		if(!running)
			break;
		switch(tok){
			case KW_BYTE: fprintf(stderr, "Keyword 'byte'.\n"); break;
			case KW_INT: fprintf(stderr, "Keyword 'int'.\n"); break;
			case KW_LONG: fprintf(stderr, "Keyword 'long'.\n"); break;
			case KW_FLOAT: fprintf(stderr, "Keyword 'float'.\n"); break;
			case KW_BOOL: fprintf(stderr, "Keyword 'bool'.\n"); break;
			case KW_IF: fprintf(stderr, "Keyword 'if'.\n"); break;
			case KW_THEN: fprintf(stderr, "Keyword 'then'.\n"); break;
			case KW_ELSE: fprintf(stderr, "Keyword 'else'.\n"); break;
			case KW_WHILE: fprintf(stderr, "Keyword 'while'.\n"); break;
			case KW_FOR: fprintf(stderr, "Keyword 'for'.\n"); break;
			case KW_READ: fprintf(stderr, "Keyword 'read'.\n"); break;
			case KW_PRINT: fprintf(stderr, "Keyword 'print'.\n"); break;
			case KW_RETURN: fprintf(stderr, "Keyword 'return'.\n"); break;
			case KW_BREAK: fprintf(stderr, "Keyword 'break'.\n"); break;
			case OPERATOR_LE: fprintf(stderr, "Operator '<='.\n"); break;
			case OPERATOR_GE: fprintf(stderr, "Operator '>='.\n"); break;
			case OPERATOR_EQ: fprintf(stderr, "Operator '=='.\n"); break;
			case OPERATOR_DIF: fprintf(stderr, "Operator '!='.\n"); break;
			case TK_IDENTIFIER: fprintf(stderr, "Identifier.\n"); break;
			case LIT_INTEGER: fprintf(stderr, "Literal Integer.\n"); break;
			case LIT_FLOAT: fprintf(stderr, "Literal Float.\n"); break;
			case LIT_TRUE: fprintf(stderr, "Literal True.\n"); break;
			case LIT_FALSE: fprintf(stderr, "Literal False.\n"); break;
			case LIT_CHAR: fprintf(stderr, "Literal Char.\n"); break;
			case LIT_STRING: fprintf(stderr, "Literal String.\n"); break;
			case TOKEN_ERROR: fprintf(stderr, "Token error.\n"); break;
			default: fprintf(stderr, "Token ascii %c\n", tok); break;
		}	
	}
	hashPrint();
}

