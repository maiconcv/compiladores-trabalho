int main(int argc, char** argv){
	int tok;

	initMe();

	if(argc < 2){
		fprintf(stderr, "File name argument is missing!\n");
		exit(1);
	}

	yyin = fopen(argv[1], "r");
	if(yyin == 0){
		fprintf(stderr, "Error opening file %s!\n", argv[1]);
		exit(2);
	}

	yyparse();
	hashPrint();
	fprintf(stderr, "Good job!\n");
	exit(0);
}

