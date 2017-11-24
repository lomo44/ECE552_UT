#include <stdio.h>

// Define variables based on the configuration of the l1 data cache config
// <name>:<nsets>:<bsize>:<assoc>:<repl>:<pref>
#define BSIZE 64

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