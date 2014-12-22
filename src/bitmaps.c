#include <stdio.h>
#include <stdlib.h>

#include "bitmaps.h"
#include "darray.h"
#include "definitions.h"
#include "runlength.h"

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
void RleMap_delete(struct RleMap *rle)
{
  int l;
  for ( l = 0 ; l < rle->nb_lines ; l++ )
    free(rle->lines[l].tokens);
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

struct RleMap *align_tokens(struct RleToken *rle)
{
  Darray *lines = da_new(sizeof(struct RleLine));
  Darray *cur_tokens = NULL;
  struct RleLine cur_line = { .line_num = 0 };
  int i;
  for ( i = 0 ; rle[i].value.char_ != END_RLE_TOKEN ; i++ )
  {
    int value = 1;
    switch ( rle[i].value.char_ )
    {
      case DEAD_RLE_TOKEN:
        value = 0;
      case ALIVE_RLE_TOKEN:
        // value = 1 by default
        if ( NULL == cur_tokens ) // First time pushing
          cur_tokens = da_new(sizeof(struct RleToken));
        struct RleToken *dest = da_alloc(cur_tokens);
        dest->value.int_ = value;
        dest->repeat = rle[i].repeat;
        break;
      case NEWLINE_RLE_TOKEN:
#define AT_newline \
        if ( NULL != cur_tokens ) \
        { \
          cur_line.tokens = da_unpack(cur_tokens, &cur_line.nb_tokens); \
          cur_tokens = NULL; \
          da_push(lines, &cur_line); \
        }
        AT_newline
        cur_line.line_num += rle[i].repeat;
        break;
    }
  }
  AT_newline;
#undef AT_newline

  struct RleMap *rle_m = malloc(sizeof(struct RleMap));
  if ( NULL == rle_m )
  {
    perror("align_tokens(): Failed to allocate memory");
    exit(1);
  }
  rle_m->lines = da_unpack(lines, &rle_m->nb_lines);
  return rle_m;
}

struct RleToken *rle_flatten(struct RleMap *rle_m)
{
  Darray *rle_da = da_new(sizeof(struct RleToken));
  for ( int i = 0 ; i < rle_m->nb_lines ; i++ )
  {
    struct RleToken t_nl = {
      .value = { .char_ = NEWLINE_RLE_TOKEN },
      .repeat = rle_m->lines[i].line_num
              - (( i == 0 ) ? 0 : rle_m->lines[i-1].line_num)
    };
    if ( t_nl.repeat > 0 )
      da_push(rle_da, &t_nl);
    for ( int j = 0 ; j < rle_m->lines[i].nb_tokens ; j++ )
    {
      struct RleToken *dest = da_alloc(rle_da);
      struct RleToken src = rle_m->lines[i].tokens[j];
      dest->repeat = src.repeat;
      dest->value.char_ = src.value.int_ ? ALIVE_RLE_TOKEN : DEAD_RLE_TOKEN;
    }
  }
  struct RleToken t_end = {
    .value = { .char_ = END_RLE_TOKEN },
    .repeat = 1
  };
  da_push(rle_da, &t_end);
  return da_unpack(rle_da, NULL);
}

BitMap *rle_to_bm(struct LifeRle rle)
{
  BitMap *bm = bm_new(RLE, align_tokens(rle.tokens));
  bm->x = rle.x;
  bm->y = rle.y;
  bm->r = rle.r;
  return bm;
}

void bm_write(FILE *file, BitMap *bm)
{
  switch ( bm->map_type )
  {
    case RLE:
      {
        struct LifeRle rle = {
          .x = bm->x,
          .y = bm->y,
          .r = bm->r,
          .tokens = rle_flatten(bm->map.rle)
        };
        LifeRle_write(file, rle);
      }
      break;
    case RAW:
      break; // unimplemented
  }
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

