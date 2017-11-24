
#include <stdio.h>
#include <stdlib.h>
#define ARRAY_SIZE 1000

int main(){
    int strides[3] = {16,64,8};
    int i = 0;
    int counter = 0;
    int* array = calloc(ARRAY_SIZE,sizeof(int));
    while(i < ARRAY_SIZE){
        array[i] +=2;
        i+=strides[counter%3]; // i advance in a paticular stride pattern {16,64,8}
        counter++;
    }
    free(array);
    return 0;
}