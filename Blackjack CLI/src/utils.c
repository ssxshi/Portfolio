#include <stdio.h>

#include "../include/utils.h"

void clear_stdin(){
    int c = 0;
    while((c = getchar()) != '\n' && c != EOF);
}

int rand_range(int min, int max){
    return (rand() % (max - min + 1)) + min;
}