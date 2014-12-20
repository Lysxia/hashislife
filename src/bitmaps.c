#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"
#include "darray.h"
#include "bitmaps.h"

/*! \param map must be a pointer to `RLE` or `char*`
  depending on the value of the tag `t`. */
BitMap *bm_new(enum MapType t, void *map)
{
  BitMap *bm = malloc(sizeof(BitMap));

  if ( NULL == bm )
  {
    perror("bm_new(): failed to allocate bm");
    exit(1);
  }

  switch ( t )
  {
    case RLE:
      bm->map.rle = map;
      break;
    case RAW:
      bm->map.raw = map;
      break;
  }

  bm->map_type = t;

  bm->corner_x = bm->corner_y = 0;
  bm->x = bm->y = 0;
  bm->r = 0;

  return bm;
}

void bm_delete(BitMap *bm)
{
  switch ( bm->map_type )
  {
    case RLE:
      if ( bm->map.rle != NULL)
        RleMap_delete(bm->map.rle);
      break;
    case RAW:
      if ( bm->map.raw != NULL )
        matrix_delete((void **) bm->map.raw, bm->y);
      break;
  }

  free(bm);
}

/*! \param `rle` and subfields must be `malloc()`ated pointers. */
void RleMap_delete(RleMap *rle)
{
  int l;
  for ( l = 0 ; l < rle->nb_lines ; l++ )
    free(rle->lines[l].encoding);
  free(rle->lines);
  free(rle);
}

void matrix_delete(void **a, int m)
{
  int i;
  for ( i = 0 ; i < m ; i++ )
    free(a[i]);
  free(a);
}

/*
Rle_line *bm_rle_newline(Darray *rle, int line_num)
{
  Rle_line *dest = da_append(rle, NULL);

  da_init(&dest->line_rle, sizeof(int));
  dest->line_num = line_num;

  return dest;
}
*/

