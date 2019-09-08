
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
	| block
	;

exp: exp opcomp exp | opsingle exp | leaf | '(' exp ')'
	;

opcomp: '+' | '-' | '*' | '/' | '<' | '>' | '.' | 'v' | OPERATOR_LE | OPERATOR_GE | OPERATOR_EQ | OPERATOR_DIF
	;

opsingle: '~'
	;

leaf: TK_IDENTIFIER isvect | init | TK_IDENTIFIER '(' arglist ')'
	;

isvect: '[' vectindex ']'
	|
	;

vectindex: TK_IDENTIFIER | LIT_INTEGER
	;

arglist: arg argrest
	|
	;

argrest: ',' arg argrest
	|
	;

arg: TK_IDENTIFIER
	;

block: '{' lcmd '}'
	;

lcmd: lcmd cmd ';'
	|
	;

%%



void yyerror(char* msg){
	fprintf(stderr, "Erro de sintaxe na linha %d!\n", getLineNumber());
	exit(3);
}
