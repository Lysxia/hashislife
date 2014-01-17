#include <stdlib.h>
#include "definitions.h"
#include "darray.h"
#include "bitmaps.h"

void bm_free(BitMap *bm)
{
  switch ( bm->map_type )
  {
    case RLE:
      if ( bm->map.rle != NULL)
        rle_map_free(bm->map.rle);
      break;
    case RAW:
      if ( bm->map.raw != NULL )
        free_matrix(bm->map.raw, bm->y);
      break;
  }

  free(bm);
}

BitMap *bm_new(enum MapType t)
{
  BitMap *bm = malloc(sizeof(BitMap));

  if ( bm == NULL )
  {
    perror("bm_new()");
    exit(1);
  }

  switch ( t )
  {
    case RLE:
      bm->map.rle = NULL;
      break;
    case RAW:
      bm->map.raw = NULL;
      break;
  }

  bm->map_type = t;

  bm->corner_x = bm->corner_y = 0;
  bm->x = bm->y = 0;
  bm->r = 0;

  return bm;
}

void rle_map_free(Rle_line *rle)
{
  int l;
  for ( l = 0 ; l < rle->array_length ; l++ )
    da_clear(&((Rle_line *) rle->da)[l].line_rle);

  da_clear(rle);
}

Rle_line *bm_rle_newline(Darray *rle, int line_num)
{
  Rle_line *dest = da_append(rle, NULL);

  da_init(&dest->line_rle, sizeof(int));
  dest->line_num = line_num;

  return dest;
}

