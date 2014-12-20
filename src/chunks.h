#ifndef CHUNKS_H
#define CHUNKS_H
#include <stdlib.h>
/*! \defgroup chunks Memory chunks */
/*!@{*/
typedef struct Chunks Chunks;
typedef struct ChunksBlocks ChunksBlocks;

//! Managed blocks
struct Chunks {
  size_t        data_size;
  int           max_len;
  ChunksBlocks *blocks;
};

//! Single block
struct ChunksBlocks {
  int           size;
  char         *block;
  ChunksBlocks *next;
};

Chunks chunks_new(const size_t data_size, const int max_len);
void   chunks_delete(Chunks);
void  *chunks_alloc(Chunks);
void   chunks_print(Chunks);
/*!@}*/
#endif
