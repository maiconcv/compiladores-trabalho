int main(){
	int tok;
	int lineno;

	initMe();

	while(isRunning()){
		tok = yylex();
		lineno = getLineNumber();

		if(!isRunning())
			break;

		switch(tok){
			case KW_BYTE: fprintf(stderr, "Line %d: Keyword 'byte'\n", lineno); break;
			case KW_INT: fprintf(stderr, "Line %d: Keyword 'int'\n", lineno); break;
			case KW_LONG: fprintf(stderr, "Line %d: Keyword 'long'\n", lineno); break;
			case KW_FLOAT: fprintf(stderr, "Line %d: Keyword 'float'\n", lineno); break;
			case KW_BOOL: fprintf(stderr, "Line %d: Keyword 'bool'\n", lineno); break;
			case KW_IF: fprintf(stderr, "Line %d: Keyword 'if'\n", lineno); break;
			case KW_THEN: fprintf(stderr, "Line %d: Keyword 'then'\n", lineno); break;
			case KW_ELSE: fprintf(stderr, "Line %d: Keyword 'else'\n", lineno); break;
			case KW_WHILE: fprintf(stderr, "Line %d: Keyword 'while'\n", lineno); break;
			case KW_FOR: fprintf(stderr, "Line %d: Keyword 'for'\n", lineno); break;
			case KW_READ: fprintf(stderr, "Line %d: Keyword 'read'\n", lineno); break;
			case KW_PRINT: fprintf(stderr, "Line %d: Keyword 'print'\n", lineno); break;
			case KW_RETURN: fprintf(stderr, "Line %d: Keyword 'return'\n", lineno); break;
			case KW_BREAK: fprintf(stderr, "Line %d: Keyword 'break'\n", lineno); break;
			case OPERATOR_LE: fprintf(stderr, "Line %d: Operator '<='\n", lineno); break;
			case OPERATOR_GE: fprintf(stderr, "Line %d: Operator '>='\n", lineno); break;
			case OPERATOR_EQ: fprintf(stderr, "Line %d: Operator '=='\n", lineno); break;
			case OPERATOR_DIF: fprintf(stderr, "Line %d: Operator '!='\n", lineno); break;
			case TK_IDENTIFIER: fprintf(stderr, "Line %d: Identifier '%s'\n", lineno, yytext); break;
			case LIT_INTEGER: fprintf(stderr, "Line %d: Literal Integer '%s'\n", lineno, yytext); break;
			case LIT_FLOAT: fprintf(stderr, "Line %d: Literal Float '%s'\n", lineno, yytext); break;
			case LIT_TRUE: fprintf(stderr, "Line %d: Literal True\n", lineno); break;
			case LIT_FALSE: fprintf(stderr, "Line %d: Literal False\n", lineno); break;
			case LIT_CHAR: fprintf(stderr, "Line %d: Literal Char %s\n", lineno, yytext); break;
			case LIT_STRING: fprintf(stderr, "Line %d: Literal String %s\n", lineno, yytext); break;
			case TOKEN_ERROR: fprintf(stderr, "Line %d: Token error '%s'\n", lineno, yytext); break;
			default: fprintf(stderr, "Line %d: Token ascii '%c'\n", lineno, tok); break;
		}	
	}

	printf("\nTotal lines: %d\n\n", lineno);
	hashPrint();
}

