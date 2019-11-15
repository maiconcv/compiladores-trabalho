#
# UFRGS - Compiladores B - Marcelo Johann - 2009/2 - Etapa 6
#
# Makefile for single compiler call
# All source files must be included from code embedded in scanner.l
# In our case, you probably need #include "hash.c" at the beginning
# and #include "main.c" in the last part of the scanner.l
#

etapa6: y.tab.o lex.yy.o main.o hash.o astree.o semantic.o tacs.o
	gcc -o etapa6 y.tab.o lex.yy.o main.o hash.o astree.o semantic.o tacs.o -Wall

y.tab.o: y.tab.c
	gcc -c y.tab.c -Wall

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

main.o: main.c
	gcc -c main.c -Wall

hash.o: hash.c
	gcc -c hash.c -Wall

astree.o: astree.c
	gcc -c astree.c -Wall

semantic.o: semantic.c
	gcc -c semantic.c -Wall

tacs.o: tacs.c
	gcc -c tacs.c -Wall

lex.yy.c: scanner.l
	lex --header-file=lex.yy.h scanner.l

y.tab.c: parser.y lex.yy.c
	yacc parser.y -d

clean:
	rm  *.o y.tab.h y.tab.c lex.yy.c lex.yy.h etapa6
