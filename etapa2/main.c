int main(){
	int tok;

	initMe();
	yyparse();
	hashPrint();
	fprintf(stderr, "Good job!");
	exit(0);
}

