#ifndef BITMAPS_H
#define BITMAPS_H

#include "definitions.h"
#include "darray.h"

enum MapType { RAW, RLE };

typedef struct
{
  union
  {
    Darray rle;
    char **raw;
  } map;

  enum MapType map_type;

  // x increasing towards the right
  // y towards the bottom
  int corner_x, corner_y; // position of top-left corner
  int x, y; // width, height
  rule r;
} BitMap;

typedef struct Rle_line
{
  Darray line_rle;
  int line_num;
} Rle_line;

void bm_free(BitMap *map);

BitMap *bm_new(enum MapType t);

void rle_map_free(Darray *rle);
Rle_line *bm_rle_newline(Darray *rle, int line_num);

#endif
