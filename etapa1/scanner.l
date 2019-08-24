
%{

#include "tokens.h"
#include "hash.c"

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

[A-Za-z_][A-Za-z_0-9]*		{ hashInsert(yytext); return TK_IDENTIFIER; }

{DIGIT}+			{ return LIT_INTEGER; }
{DIGIT}+"."{DIGIT}+		{ return LIT_FLOAT; }
"true"				return LIT_TRUE;
"false"				return LIT_FALSE;

[-+*/();]			return yytext[0];
[\t]
"\n"				lineNumber++;
"//".*
.
"/*"				BEGIN(COMMENT);
<COMMENT>"*/"			BEGIN(INITIAL);
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

#include "main.c"

