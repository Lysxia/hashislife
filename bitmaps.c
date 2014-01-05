#include <stdlib.h>
#include "definitions.h"
#include "darray.h"
#include "bitmaps.h"

void bm_free(BitMap *map)
{
  switch ( map->map_type )
  {
    case RLE:
      rle_map_free(&map->map.rle);
      break;
    case RAW:
      if ( map->map.raw != NULL )
      {
        int i;
        for ( i = 0 ; i < map->y ; i++ )
          free(map->map.raw[i]);
        free(map->map.raw);
      }
      break;
  }
}

BitMap *bm_new(enum MapType t)
{
  BitMap *map = malloc(sizeof(BitMap));

  if ( map == NULL )
    return NULL;

  switch (t)
  {
    case RLE:
      da_init(&map->map.rle, sizeof(Rle_line));
      break;
    case RAW:
      map->map.raw = NULL;
      break;
  }

  map->map_type = t;

  map->corner_x = map->corner_y = 0;
  map->x = map->y = 0;
  map->r = 0;

  return map;
}

void rle_map_free(Darray *rle)
{
  int l;
  for (l = 0 ; l < rle->array_length ; l++)
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

