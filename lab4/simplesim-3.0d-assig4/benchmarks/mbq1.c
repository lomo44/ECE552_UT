#include <stdio.h>

// <name>:<nsets>:<bsize>:<assoc>:<repl>:<pref>
#define BSIZE 64
//when the SDTEP is BSIZE, next line prefetch should perform well and have very low missrate
#define STEP (BSIZE)
#define ARRAY_SIZE 1000000

int main(void) {
  char arr[ARRAY_SIZE];
  int sum = 16; 
  int i;
  for (i = 0; i < ARRAY_SIZE; i += STEP) {
    sum += arr[i];
  }
  return 0;
}