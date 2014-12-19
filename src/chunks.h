#ifndef CHUNKS_H
#define CHUNKS_H
/*! \defgroup chunks Memory chunks */
/*!@{*/
typedef struct Chunks Chunks;
typedef struct ChunksBlocks ChunksBlocks;

//! Managed blocks
struct Chunks {
  const size_t  chunks_data_size;
  const int     chunks_max_len;
  ChunksBlocks *chunks_blocks;
};

//! Single block
struct ChunksBlocks {
  int           chunksb_size;
  char         *chunksb_block;
  ChunksBlocks *chunksb_next;
};

Chunks chunks_new(const size_t data_size, const int max_len);
void chunks_delete(Chunks);
void *chunks_alloc(Chunks);
/*!@}*/
#endif
