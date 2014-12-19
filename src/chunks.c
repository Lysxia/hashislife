#include <stdlib.h>
#include "chunks.h"

//! Create single block
ChunksBlocks *chunksblocks_new(size_t n)
{
  ChunksBlocks *cb = malloc(sizeof(ChunksBlocks));

  if ( cb == NULL )
    return NULL;

  cb->chunksb_size = 0;
  cb->chunksb_block = malloc(n);
  cb->chunksb_next = NULL;

  if ( cb->chunksb_block == NULL ) {
    free(cb);
    return NULL;
  }
  else
    return cb;
}

/*! In case of failure, the `.chunks_blocks` field is set to `NULL`
*/
Chunks chunks_new(const size_t data_size, const int max_len)
{
  Chunks c = {
    .chunks_data_size = data_size,
    .chunks_max_len = max_len,
    .chunks_blocks = chunksblocks_new(data_size * max_len) };

  return c;
}

/*! Do not call twice on a successfully created chunk,
  or ever on a failed one.
*/
void chunks_delete(Chunks c)
{
  ChunksBlocks *cb = c.chunks_blocks;
  while ( cb != NULL )
  {
    ChunksBlocks *cb_next = cb->chunksb_next;
    free(cb->chunksb_block);
    free(cb);
    cb = cb_next;
  }
}

/*! Allocates a unit of memory */
void *chunks_alloc(Chunks c)
{
  if (    (unsigned) c.chunks_blocks->chunksb_size
       == c.chunks_data_size * c.chunks_max_len ) // Current block is full
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

    char *block = malloc(c.chunks_data_size * c.chunks_max_len);

    if ( block == NULL ) {
      free(cb);
      return NULL;
    }

    *cb = *c.chunks_blocks;

    c.chunks_blocks->chunksb_size = 0;
    c.chunks_blocks->chunksb_block = block;
    c.chunks_blocks->chunksb_next = cb;
  }

  void *p
    = &c.chunks_blocks->chunksb_block[c.chunks_blocks->chunksb_size];
  c.chunks_blocks->chunksb_size += c.chunks_data_size;
  return p;
}

