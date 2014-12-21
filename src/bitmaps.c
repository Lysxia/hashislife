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

RleMap *align_tokens(struct RleToken *rle)
{
  Darray *lines = da_new(sizeof(struct RleLine));
  Darray *cur_tokens = NULL;
  struct RleLine cur_line = { .line_num = 0 };
  int i;
  for ( i = 0 ; rle[i].value != END_RLE_TOKEN ; i++ )
  {
    switch ( rle[i].value )
    {
      case DEAD_RLE_TOKEN:
      case ALIVE_RLE_TOKEN:
        if ( NULL == cur_tokens )
          cur_tokens = da_new(sizeof(struct RleToken));
        da_push(cur_tokens, &rle[i]);
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

  RleMap *rle_m = malloc(sizeof(RleMap));
  if ( NULL == rle_m )
  {
    perror("align_tokens(): Failed to allocate memory");
    exit(1);
  }
  rle_m->lines = da_unpack(lines, &rle_m->nb_lines);
  return rle_m;
}

void RleMap_write(FILE *file, RleMap *rle_m)
{
  struct TokenWriter tw = {
    .file = file,
    .line_length = 0
  };
  for ( int i = 0 ; i < rle_m->nb_lines ; i++ )
  {
    struct RleToken t_nl = {
      .value = NEWLINE_RLE_TOKEN,
      .repeat = rle_m->lines[i].line_num
              - (( i == 0 ) ? 0 : rle_m->lines[i-1].line_num)
    };
    write_one_token(&tw, t_nl);
    for ( int j = 0 ; j < rle_m->lines[i].nb_tokens ; j++ )
      write_one_token(&tw, rle_m->lines[i].tokens[j]);
  }
  struct RleToken t_end = {
    .value = END_RLE_TOKEN,
    .repeat = 1
  };
  write_one_token(&tw, t_end);
  fputc('\n', file);
  fflush(file);
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

