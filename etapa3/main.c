#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

void initMe(void);
int yyparse();
extern FILE* yyin;
void outputFile(FILE* file);

int main(int argc, char** argv){
	initMe();

	if(argc < 3){
		fprintf(stderr, "File name argument is missing! Please try ./<exec> <inputfile> <outputfile>\n");
		exit(1);
	}

	yyin = fopen(argv[1], "r");
	if(yyin == 0){
		fprintf(stderr, "Error opening file %s!\n", argv[1]);
		exit(2);
	}

	FILE* output = fopen(argv[2], "w");
	if(output == 0){
		fprintf(stderr, "Error opening file %s!\n", argv[2]);
		exit(2);
	}

	outputFile(output);
	yyparse();
	hashPrint();
	fprintf(stderr, "Good job!\n");
	exit(0);
}

