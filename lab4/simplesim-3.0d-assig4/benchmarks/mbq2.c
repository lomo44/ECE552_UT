
#include <stdio.h>
#include <stdlib.h>
#define ARRAY_SIZE 1000000
//when the STRIDE is BSIZE, next line prefetch should perform well and have very low missrate
//with stride prefetcher, it should find out stride is constant and prefetch the correct mempry
#define STRIDE 512
int main(){
    int i = 0;
    int* array = calloc(ARRAY_SIZE,sizeof(int));
    while(i < ARRAY_SIZE){
        array[i] +=2;
        // will need array [i + Stride]
        i+=STRIDE;
    }
    free(array);
    return 0;
}