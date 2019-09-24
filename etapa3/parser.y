
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
%token<symbol> LIT_TRUE
%token<symbol> LIT_FALSE
%token<symbol> LIT_CHAR
%token<symbol> LIT_STRING

%token TOKEN_ERROR


%type<ast> exp
%type<ast> cmd

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

cmd: TK_IDENTIFIER '=' exp			{ $$ = astCreate(AST_ASSIGN, $1, $3, 0, 0, 0); }
	| TK_IDENTIFIER '[' exp ']' '=' exp	{ $$ = 0; }
	| KW_READ TK_IDENTIFIER			{ $$ = 0; }
	| KW_PRINT printarglist			{ $$ = 0; }
	| KW_RETURN exp				{ $$ = 0; }
	| KW_IF '(' exp ')' KW_THEN cmd haselse	{ $$ = 0; }
	| KW_WHILE '(' exp ')' cmd		{ $$ = 0; }
	| KW_BREAK				{ $$ = 0; }
	| KW_FOR '(' TK_IDENTIFIER ':' exp ',' exp ',' exp ')' cmd	{ $$ = 0; }
	| block					{ $$ = 0; }
	|					{ $$ = 0; }
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
	| LIT_TRUE			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| LIT_FALSE			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| LIT_CHAR			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| LIT_STRING			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| TK_IDENTIFIER			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| TK_IDENTIFIER '[' exp ']'	{ $$ = astCreate(AST_VECTREAD, $1, $3, 0, 0, 0); }
	| TK_IDENTIFIER '(' arglist ')' { $$ = 0; }
	| exp '+' exp			{ $$ = astCreate(AST_ADD, 0, $1, $3, 0, 0); }
	| exp '-' exp			{ $$ = astCreate(AST_SUB, 0, $1, $3, 0, 0); }
	| exp '*' exp			{ $$ = astCreate(AST_MUL, 0, $1, $3, 0, 0); }
	| exp '/' exp			{ $$ = astCreate(AST_DIV, 0, $1, $3, 0, 0); }
	| exp '<' exp			{ $$ = astCreate(AST_LT, 0, $1, $3, 0, 0); }
	| exp '>' exp			{ $$ = astCreate(AST_GT, 0, $1, $3, 0, 0); }
	| exp '.' exp			{ $$ = astCreate(AST_AND, 0, $1, $3, 0, 0); }
	| exp 'v' exp			{ $$ = astCreate(AST_OR, 0, $1, $3, 0, 0); }
	| exp OPERATOR_LE exp		{ $$ = astCreate(AST_LE, 0, $1, $3, 0, 0); }
	| exp OPERATOR_GE exp		{ $$ = astCreate(AST_GE, 0, $1, $3, 0, 0); }
	| exp OPERATOR_EQ exp		{ $$ = astCreate(AST_EQ, 0, $1, $3, 0, 0); }
	| exp OPERATOR_DIF exp		{ $$ = astCreate(AST_DIF, 0, $1, $3, 0, 0); }
	| '~' exp			{ $$ = astCreate(AST_NOT, 0, $2, 0, 0, 0); }
	| '(' exp ')'			{ $$ = astCreate(AST_BRACKETS, 0, $2, 0, 0, 0); }
	;

arglist: exp argrest
	|
	;

argrest: ',' exp argrest
	|
	;

block: '{' lcmd '}'
	;

lcmd: cmd ';' lcmd			{ astPrint($1, 0); }
	| cmd				{ astPrint($1, 0); }
	;

%%



int yyerror(const char* msg){
	fprintf(stderr, "Erro de sintaxe na linha %d!\n", getLineNumber());
	exit(3);
}
