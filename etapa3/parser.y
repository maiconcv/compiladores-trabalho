
%{

#include <stdio.h>
#include <stdlib.h>
#include "lex.yy.h"
#include "hash.h"
#include "astree.h"

int yyerror(const char* msg);
int getLineNumber(void);
//int yylex();

%}


%union{
	HASH_NODE* symbol;
	AST* ast;
}


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

%token<symbol> TK_IDENTIFIER

%token<symbol> LIT_INTEGER
%token<symbol> LIT_FLOAT
%token LIT_TRUE
%token LIT_FALSE
%token LIT_CHAR
%token LIT_STRING

%token TOKEN_ERROR


%type<ast> exp

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

cmd: TK_IDENTIFIER '=' exp	{ astPrint($3, 0); }
	| TK_IDENTIFIER '[' exp ']' '=' exp
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

exp: LIT_INTEGER 			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| LIT_FLOAT			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| LIT_TRUE			{ $$ = 0; }
	| LIT_FALSE			{ $$ = 0; }
	| LIT_CHAR			{ $$ = 0; }
	| LIT_STRING			{ $$ = 0; }
	| TK_IDENTIFIER			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| TK_IDENTIFIER '[' exp ']'	{ $$ = 0; }
	| TK_IDENTIFIER '(' arglist ')' { $$ = 0; }
	| exp '+' exp			{ $$ = astCreate(AST_ADD, 0, $1, $3, 0, 0); }
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
	| '~' exp			{ $$ = 0; }
	| '(' exp ')'			{ $$ = 0; }
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
