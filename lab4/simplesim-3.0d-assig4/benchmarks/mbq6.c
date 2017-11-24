
#include <stdio.h>
#include <stdlib.h>
#define ARRAY_SIZE 1000000

int main(){
    int strides[3] = {16,128,4};
    int i = 0;
    int counter = 0;
    int* array = calloc(ARRAY_SIZE,sizeof(int));
    int sum = 0;
    while(counter < ARRAY_SIZE){
        //array[counter%3] +=2;// i advance in a paticular stride pattern {16,64,8}
        sum += array[counter%3];
        counter++;
    }
    
    free(array);
    return 0;
}