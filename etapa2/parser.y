
%{

#include <stdio.h>
#include <stdlib.h>

%}

%token KW_BYTE
%token KW_INT
%token KW_LONG
%token KW_FLOAT
%token KW_BOOL

%token KW_IF
%token KW_THEN
%token KW_ELSE
%token KW_WHILE
%token KW_FOR
%token KW_READ
%token KW_PRINT
%token KW_RETURN
%token KW_BREAK

%token OPERATOR_LE
%token OPERATOR_GE
%token OPERATOR_EQ
%token OPERATOR_DIF

%token TK_IDENTIFIER

%token LIT_INTEGER
%token LIT_FLOAT
%token LIT_TRUE
%token LIT_FALSE
%token LIT_CHAR
%token LIT_STRING

%token TOKEN_ERROR



%%



programa: programa decl
	|
	;

decl: vardecl | fundecl | vectdecl
	;

vardecl: type TK_IDENTIFIER '=' init ';'
	;

type: KW_BYTE | KW_INT | KW_LONG | KW_FLOAT | KW_BOOL
	;

init: LIT_INTEGER | LIT_FLOAT | LIT_TRUE | LIT_FALSE | LIT_CHAR | LIT_STRING
	;

fundecl: type TK_IDENTIFIER '(' paramlist ')' cmd
	;

vectdecl: type TK_IDENTIFIER '[' LIT_INTEGER ']' vectatrib ';'
	;

vectatrib: ':' lstinit
	|
	;

lstinit: init lstinit
	|
	;

paramlist: param paramrest
	|
	;

paramrest: ',' param paramrest
	|
	;

param: type TK_IDENTIFIER
	;

cmd: TK_IDENTIFIER isvect '=' exp
	| TK_IDENTIFIER '=' exp
	| KW_READ TK_IDENTIFIER
	| KW_PRINT printarglist
	| KW_RETURN exp
	| KW_IF '(' exp ')' KW_THEN cmd haselse
	| KW_WHILE '(' exp ')' cmd
	| KW_BREAK
	| KW_FOR '(' TK_IDENTIFIER ':' exp ',' exp ',' exp ')' cmd
	| block
	;

haselse: KW_ELSE cmd
	|
	;

printarglist: exp printargrest
	;

printargrest: printargsep exp printargrest
	|
	;

printargsep: ','
	|
	;

exp: exp '+' exp
	| exp '-' exp
	| exp '*' exp
	| exp '/' exp
	| exp '<' exp
	| exp '>' exp
	| exp '.' exp
	| exp 'v' exp
	| exp OPERATOR_LE exp
	| exp OPERATOR_GE exp
	| exp OPERATOR_EQ exp
	| exp OPERATOR_DIF exp
	| '~' exp
	| leaf
	| '(' exp ')'
	;

leaf: TK_IDENTIFIER isvect | init | TK_IDENTIFIER '(' arglist ')'
	;

isvect: '[' exp ']'
	|
	;

arglist: exp argrest
	|
	;

argrest: ',' exp argrest
	|
	;

block: '{' lcmd '}'
	;

lcmd: cmd ';' lcmd | cmd
	|
	;

%%



int yyerror(const char* msg){
	fprintf(stderr, "Erro de sintaxe na linha %d!\n", getLineNumber());
	exit(3);
}
