#include "litstr_to_var.h"

int nextEmptyPos = 0;
LITSTR_TO_VAR match[1000];

void addMatch(char* text, int counter){
        match[nextEmptyPos].text = (char*)malloc(sizeof(text));
        strcpy(match[nextEmptyPos].text, text);
        match[nextEmptyPos].counter = counter;
        nextEmptyPos++;
}

int findCounter(char* text){
        for(int i = 0; i < nextEmptyPos; i++){
                if(strcmp(text, match[i].text) == 0)
                        return match[i].counter;
        }
        return 0;
}
