
#include <stdio.h>
#include <stdlib.h>
#define ARRAY_SIZE 1000
#define STRIDE 2
int main(){
    int i = 0;
    int* array = calloc(ARRAY_SIZE,sizeof(int));
    while(i < ARRAY_SIZE){
        array[i] +=2;
        i+=STRIDE;
    }
    free(array);
}