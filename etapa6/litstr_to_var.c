#include "litstr_to_var.h"

int nextEmptyPos = 0;
LITSTR_TO_VAR match[1000];

void addMatch(char* text, int counter){
        match[nextEmptyPos].text = (char*)malloc(strlen(text)*sizeof(char)+1);
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

int floatToBinaryToInt(char* text){
        float f = strtof(text, 0);
        int* p = (int*)&f;

        /*for(int i = sizeof(int)*8 - 1; i >= 0; i--)
                printf("%d", (*p) >> i & 1);*/

        int answer = 0;
        for(int i = 0; i < sizeof(int)*8 - 1; i++)
                answer += ((*p) >> i & 1) * pow(2, i);
        //printf("\n%d\n", answer);

        return answer;
}
