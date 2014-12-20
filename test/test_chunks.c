#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "chunks.h"

int main(void) {
  const int n = 5;
  Chunks c = chunks_new(sizeof(int), n);
  chunks_print(c);
  int i, k, j = 0;
  for ( k = 0 ; k < 7 ; k++ ) {
    for ( i = 0 ; i < 3 ; i++, j++ ) {
      * (int *) chunks_alloc(c) = j;
    }
    chunks_print(c);
  }
  return 0;
}

