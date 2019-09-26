
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
%type<ast> lcmd
%type<ast> block
%type<ast> vardecl
%type<ast> fundecl
%type<ast> vectdecl
%type<ast> programa
%type<ast> type
%type<ast> decl
%type<ast> init
%type<ast> vectatrib
%type<ast> lstinit
%type<ast> param
%type<ast> paramrest
%type<ast> paramlist
%type<ast> printarglist
%type<ast> printargrest
%type<ast> argrest
%type<ast> arglist

%left '+' '-'
%left '*' '/'

%%

begin: programa					{ astPrint($1, 0); }
	;

programa: programa decl				{ $$ = astCreate(AST_LDECL, 0, $1, $2, 0, 0); }
	|					{ $$ = 0; }
	;

decl: vardecl | fundecl | vectdecl
	;

vardecl: type TK_IDENTIFIER '=' init ';'	{ $$ = astCreate(AST_VARDECL, $2, $1, $4, 0, 0); }
	;

type: KW_BYTE				{ $$ = astCreate(AST_TYPEBYTE, 0, 0, 0, 0, 0); }
	| KW_INT			{ $$ = astCreate(AST_TYPEINT, 0, 0, 0, 0, 0); }
	| KW_LONG			{ $$ = astCreate(AST_TYPELONG, 0, 0, 0, 0, 0); }
	| KW_FLOAT			{ $$ = astCreate(AST_TYPEFLOAT, 0, 0, 0, 0, 0); }
	| KW_BOOL			{ $$ = astCreate(AST_TYPEBOOL, 0, 0, 0, 0, 0); }
	;

init: LIT_INTEGER			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| LIT_FLOAT			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| LIT_TRUE			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| LIT_FALSE			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| LIT_CHAR			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	| LIT_STRING			{ $$ = astCreate(AST_SYMBOL, $1, 0, 0, 0, 0); }
	;

fundecl: type TK_IDENTIFIER '(' paramlist ')' cmd	{ $$ = astCreate(AST_FUNDECL, $2, $1, $4, $6, 0); }
	;

vectdecl: type TK_IDENTIFIER '[' LIT_INTEGER ']' vectatrib ';'	{ AST* vectSizeASTNode = astCreate(AST_SYMBOL, $4, 0, 0, 0, 0);
								  $$ = astCreate(AST_VECTDECL, $2, $1, vectSizeASTNode, $6, 0); }
	;

vectatrib: ':' lstinit			{ $$ = astCreate(AST_VECTINIT, 0, $2, 0, 0, 0); }
	|				{ $$ = 0; }
	;

lstinit: init lstinit			{ $$ = astCreate(AST_LSTLIT, 0, $1, $2, 0, 0); }
	|				{ $$ = 0; }
	;

paramlist: param paramrest		{ $$ = astCreate(AST_LSTPARAM, 0, $1, $2, 0, 0); }
	|				{ $$ = 0; }
	;

paramrest: ',' param paramrest		{ $$ = astCreate(AST_LSTPARAM, 0, $2, $3, 0, 0); }
	|				{ $$ = 0; }
	;

param: type TK_IDENTIFIER		{ $$ = astCreate(AST_PARAM, $2, $1, 0, 0, 0); }
	;

cmd: TK_IDENTIFIER '=' exp				{ $$ = astCreate(AST_ASSIGN, $1, $3, 0, 0, 0); }
	| TK_IDENTIFIER '[' exp ']' '=' exp		{ $$ = astCreate(AST_VECTASSIGN, $1, $3, $6, 0, 0); }
	| KW_READ TK_IDENTIFIER				{ $$ = astCreate(AST_READ, $2, 0, 0, 0, 0); }
	| KW_PRINT printarglist				{ $$ = astCreate(AST_PRINT, 0, $2, 0, 0, 0); }
	| KW_RETURN exp					{ $$ = astCreate(AST_RETURN, 0, $2, 0, 0, 0); }
	| KW_IF '(' exp ')' KW_THEN cmd			{ $$ = astCreate(AST_IF, 0, $3, $6, 0, 0); }
	| KW_IF '(' exp ')' KW_THEN cmd KW_ELSE cmd	{ $$ = astCreate(AST_IFELSE, 0, $3, $6, $8, 0); }
	| KW_WHILE '(' exp ')' cmd			{ $$ = astCreate(AST_WHILE, 0, $3, $5, 0, 0); }
	| KW_BREAK					{ $$ = astCreate(AST_BREAK, 0, 0, 0, 0, 0); }
	| KW_FOR '(' TK_IDENTIFIER ':' exp ',' exp ',' exp ')' cmd	{ $$ = astCreate(AST_FOR, $3, $5, $7, $9, $11); }
	| block						{ $$ = $1; }
	|						{ $$ = 0; }
	;

printarglist: exp printargrest				{ $$ = astCreate(AST_PRINTARG, 0, $1, $2, 0, 0); }
	;

printargrest: printargsep exp printargrest		{ $$ = astCreate(AST_PRINTARG, 0, $2, $3, 0, 0); }
	|						{ $$ = 0; }
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
	| TK_IDENTIFIER '(' arglist ')' { $$ = astCreate(AST_FUNCALL, $1, $3, 0, 0, 0); }
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

arglist: exp argrest			{ $$ = astCreate(AST_FUNARG, 0, $1, $2, 0, 0); }
	|				{ $$ = 0; }
	;

argrest: ',' exp argrest		{ $$ = astCreate(AST_FUNARG, 0, $2, $3, 0, 0); }
	|				{ $$ = 0; }
	;

block: '{' lcmd '}'			{ $$ = $2; }
	;

lcmd: cmd ';' lcmd			{ $$ = astCreate(AST_LCMD, 0, $1, $3, 0, 0); }
	| cmd				{ $$ = astCreate(AST_LCMD, 0, $1, 0, 0, 0); }
	;

%%



int yyerror(const char* msg){
	fprintf(stderr, "Erro de sintaxe na linha %d!\n", getLineNumber());
	exit(3);
}
