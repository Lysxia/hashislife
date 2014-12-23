#include <stdio.h>
#include <stdlib.h>

#include "bitmaps.h"
#include "darray.h"
#include "definitions.h"
#include "runlength.h"

BitMap *bm_new_rle(struct LifeRle rle)
{
  BitMap *bm = malloc(sizeof(*bm));
  if ( NULL == bm )
  {
    perror("bm_new_rle()");
    return NULL;
  }
  bm->map_type = RLE;
  bm->map.rle = align_tokens(rle.tokens);
  if ( bm->map.rle.nb_lines < 0 )
  {
    free(bm);
    perror("bm_new_rle()");
    return NULL;
  }
  bm->x = rle.x;
  bm->y = rle.y;
  bm->r = rle.r;
  return bm;
}

BitMap *bm_new_mat(char **mat, int m, int n)
{
  BitMap *bm = malloc(sizeof(*bm));
  if ( NULL == bm )
  {
    perror("bm_new_rle()");
    return NULL;
  }
  bm->map_type = MAT;
  bm->map.mat = mat;
  bm->x = m;
  bm->y = n;
  return bm;
}

void **matrix_new(size_t sz, int m, int n)
{
  void **mat = malloc(m * sizeof(*mat));
  if ( NULL == mat )
  {
    perror("matrix_new()");
    return NULL;
  }
  void *mat_ = malloc(m * n * sz);
  if ( NULL == mat_ )
  {
    free(mat);
    perror("matrix_new()");
    return NULL;
  }
  for ( int i = 0 ; i < m ; i++ )
  {
    mat[i] = mat_ + i * n;
  }
  return mat;
}

void bm_delete(BitMap *bm)
{
  switch ( bm->map_type )
  {
    case RLE:
      RleMap_delete(bm->map.rle);
      break;
    case MAT:
      if ( bm->map.mat != NULL )
        matrix_delete((void **) bm->map.mat, bm->y);
      break;
  }

  free(bm);
}

/*! \param `rle` and subfields must be `malloc()`ated pointers. */
void RleMap_delete(struct RleMap rle)
{
  for ( int l = 0 ; l < rle.nb_lines ; l++ )
    free(rle.lines[l].tokens);
  free(rle.lines);
}

void matrix_delete(void **a, int m)
{
  int i;
  for ( i = 0 ; i < m ; i++ )
    free(a[i]);
  free(a);
}

void matrix_write(FILE *file, char **mat, int m, int n)
{
  for ( int i = 0 ; i < m ; i++ )
  {
    fwrite(mat[i], sizeof(char), n, file);
    fputc('\n', file);
  }
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
    case MAT:
      break; // unimplemented
  }
}

struct RleMap align_tokens(struct RleToken *rle)
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

  struct RleMap rle_m;
  rle_m.lines = da_unpack(lines, &rle_m.nb_lines);
  return rle_m;
}

struct RleToken *rle_flatten(struct RleMap rle_m)
{
  Darray *rle_da = da_new(sizeof(struct RleToken));
  for ( int i = 0 ; i < rle_m.nb_lines ; i++ )
  {
    struct RleToken t_nl = {
      .value = { .char_ = NEWLINE_RLE_TOKEN },
      .repeat = rle_m.lines[i].line_num
              - (( i == 0 ) ? 0 : rle_m.lines[i-1].line_num)
    };
    if ( t_nl.repeat > 0 )
      da_push(rle_da, &t_nl);
    for ( int j = 0 ; j < rle_m.lines[i].nb_tokens ; j++ )
    {
      struct RleToken *dest = da_alloc(rle_da);
      struct RleToken src = rle_m.lines[i].tokens[j];
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

