#include <stdio.h>
#include <stdlib.h>

int main(){
    int a[1000000]={0};
    int i,j;
    int result;
    for(i=0;i<1000000;i++){
        j = i%8;
        result = a[j];
    }
    return result;
}
