//
// Created by lizhuan1 on 13/10/17.
//

#include <stdlib.h>
#include <stdio.h>
int main(){
    register int i;
    register int j = 0;
    for (i = 0 ; i < 10000; i++){
        if( j % 2 != 0){
            j+=2;
        }
        j++;
    }
}
