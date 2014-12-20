#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"
#include "chunks.h"

//! Create single block
ChunksBlocks *chunksblocks_new(size_t n)
{
  ChunksBlocks *cb = malloc(sizeof(ChunksBlocks));

  if ( cb == NULL )
    return NULL;

  cb->size = 0;
  cb->block = malloc(n);
  cb->next = NULL;

  if ( cb->block == NULL ) {
    free(cb);
    return NULL;
  }
  else
    return cb;
}

/*! In case of failure, the `blocks` field is set to `NULL`
*/
Chunks chunks_new(const size_t data_size, const int max_len)
{
  Chunks c = {
    .data_size = data_size,
    .max_len = max_len,
    .blocks = chunksblocks_new(data_size * max_len) };

  return c;
}

/*! Do not call twice on a successfully created chunk,
  or ever on a failed one.
*/
void chunks_delete(Chunks c)
{
  ChunksBlocks *cb = c.blocks;
  while ( cb != NULL )
  {
    ChunksBlocks *cb_next = cb->next;
    free(cb->block);
    free(cb);
    cb = cb_next;
  }
}

/*! Allocates a unit of memory */
void *chunks_alloc(Chunks c)
{
  if (    (unsigned) c.blocks->size
       == c.data_size * c.max_len ) // Current block is full
  {
    /* Create new block
      A little hack to get around the fact that c is passed by value:
      the new block takes the fields of the filled block,
      while the latter is cleared for a new one...
      I wonder how much one gains over a pointer argument.
    */
    ChunksBlocks *cb = malloc(sizeof(ChunksBlocks));

    if ( cb == NULL )
      return NULL;

    char *block = malloc(c.data_size * c.max_len);

    if ( block == NULL ) {
      free(cb);
      return NULL;
    }

    *cb = *c.blocks;

    c.blocks->size = 0;
    c.blocks->block = block;
    c.blocks->next = cb;
  }

  void *p = &c.blocks->block[c.blocks->size];
  c.blocks->size += c.data_size;
  return p;
}

/*! Debug information on stderr
*/
void chunks_print(Chunks c) {
  fprintf(stderr, "sizeof: %zu, length: %d\n", c.data_size, c.max_len);
  ChunksBlocks *cb;
  for ( cb = c.blocks ; cb != NULL ; cb = cb->next ) {
    int i;
    for ( i = 0 ; i < cb->size ; i++ ) {
      if ( i % c.data_size == 0 && i != 0 )
        fprintf(stderr, " ");
      fprintf(stderr, "%c%c", digit_to_char(cb->block[i]/16),
                     digit_to_char(cb->block[i]%16));
    }
    fprintf(stderr, "\n");
  }
}

