#ifndef BITMAPS_H
#define BITMAPS_H

#include "definitions.h"
#include "darray.h"

enum MapType { RAW, RLE };

typedef struct Rle_line
{
  int *line_rle;
  int line_length;
  int line_num;
} Rle_line;

typedef struct
{
  union
  {
    struct
    {
      Rle_line *rle_lines;
      int rle_lines_c;
    } rle;

    char **raw;
  } map;

  enum MapType map_type;

  // x increasing towards the right
  // y towards the bottom
  int x, y;
  int corner_x, corner_y; // position of top-left corner
  rule r;
} BitMap;

void bm_free(BitMap *map);

BitMap *bm_new(enum MapType t);

void rle_map_free(Rle_line *rle);

//Rle_line *bm_rle_newline(Darray *rle, int line_num);

#endif
