#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "chunks.h"

void show_chunk(Chunks c) {
  printf("sizeof: %zu, length: %d\n", c.chunks_data_size, c.chunks_max_len);
  ChunksBlocks *cb;
  for ( cb = c.chunks_blocks ; cb != NULL ; cb = cb->chunksb_next ) {
    int i;
    for ( i = 0 ; i < cb->chunksb_size ; i++ ) {
      if ( i % c.chunks_data_size == 0 && i != 0 )
        printf(" ");
      printf("%c%c", digit_to_char(cb->chunksb_block[i]/16),
                     digit_to_char(cb->chunksb_block[i]%16));
    }
    printf("\n");
  }
}

int main(void) {
  const int n = 5;
  Chunks c = chunks_new(sizeof(int), n);
  show_chunk(c);
  int i, k, j = 0;
  for ( k = 0 ; k < 7 ; k++ ) {
    for ( i = 0 ; i < 3 ; i++, j++ ) {
      * (int *) chunks_alloc(c) = j;
    }
    show_chunk(c);
  }
  return 0;
}

