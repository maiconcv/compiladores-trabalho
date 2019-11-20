#ifndef COMP_LITSTR_H
#define COMP_LITSTR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LITSTR_VAR_NAME "lItsTtriNnG"

typedef struct litstr_to_var{
        int counter;
        char* text;
} LITSTR_TO_VAR;

void addMatch(char* text, int counter);
int findCounter(char* text);

#endif // COMP_LITSTR_H
