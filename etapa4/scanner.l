
%{

#include "hash.h"
#include "astree.h"
#include "y.tab.h"

int lineNumber = 1;

%}

%x COMMENT

DIGIT [0-9]

%%

"byte"				return KW_BYTE;
"int"				return KW_INT;
"long"				return KW_LONG;
"float"				return KW_FLOAT;
"bool"				return KW_BOOL;

"if"				return KW_IF;
"then"				return KW_THEN;
"else"				return KW_ELSE;
"while"				return KW_WHILE;
"for"				return KW_FOR;
"read"				return KW_READ;
"print"				return KW_PRINT;
"return"			return KW_RETURN;
"break"				return KW_BREAK;

"<="				return OPERATOR_LE;
">="				return OPERATOR_GE;
"=="				return OPERATOR_EQ;
"!="				return OPERATOR_DIF;

{DIGIT}+			{ yylval.symbol = hashInsert(yytext); return LIT_INTEGER; }
{DIGIT}+"."{DIGIT}+		{ yylval.symbol = hashInsert(yytext); return LIT_FLOAT; }
"."{DIGIT}+			{ yylval.symbol = hashInsert(yytext); return LIT_FLOAT; }
{DIGIT}+"."			{ yylval.symbol = hashInsert(yytext); return LIT_FLOAT; }
"TRUE"				{ yylval.symbol = hashInsert(yytext); return LIT_TRUE; }
"FALSE"				{ yylval.symbol = hashInsert(yytext); return LIT_FALSE; }
\'.\'				{ yylval.symbol = hashInsert(yytext); return LIT_CHAR; }
\"(\\.|[^"])*\"			{ yylval.symbol = hashInsert(yytext); return LIT_STRING; }

[,;:()\[\]{}+\-*/<>=v.~&$#]	return yytext[0];

[A-Za-z_][A-Za-z_0-9]*		{ yylval.symbol = hashInsert(yytext); return TK_IDENTIFIER; }

[ \t]
"\n"				lineNumber++;
"//".*
.				return TOKEN_ERROR;

"###"				BEGIN(COMMENT);
<COMMENT>"###"			BEGIN(INITIAL);
<COMMENT>"\n"			lineNumber++;
<COMMENT>. 


%%

int running = 1;

int yywrap(){
	running = 0;
	return 1;
}

void initMe(void){
	hashInit();
}

int isRunning(void){
	return running;
}

int getLineNumber(void){
	return lineNumber;
}
