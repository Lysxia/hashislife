#include <stdio.h>
#include <stdlib.h>

#include "bitmaps.h"
#include "darray.h"
#include "definitions.h"
#include "runlength.h"

int push_new_line(DArray *lines, DArray *token_line, size_t line_num);

int bm_new_rle(BitMap *bm, struct LifeRle rle)
{
  *bm = (BitMap) {
    .map_type = RLE,
    .x = rle.x,
    .y = rle.y,
    .r = rle.r,
  };
  return ( align_tokens(&bm->map.rle, rle.tokens) );
}

void bm_new_mat(BitMap *bm, char **mat, size_t m, size_t n)
{
  *bm = (BitMap) {
    .map_type = MAT,
    .map.mat = mat,
    .x = m,
    .y = n,
  };
}

void **matrix_new(size_t sz, size_t m, size_t n)
{
  void **mat = malloc(m * sizeof(*mat));
  if ( NULL == mat )
  {
    perror("matrix_new()");
    return NULL;
  }
  char *mat_ = malloc(m * n * sz);
  if ( NULL == mat_ )
  {
    free(mat);
    perror("matrix_new()");
    return NULL;
  }
  for ( size_t i = 0 ; i < m ; i++ )
  {
    mat[i] = mat_ + i * n * sz;
  }
  return mat;
}

void bm_delete(BitMap *bm)
{
  switch ( bm->map_type )
  {
    case RLE:
      RleMap_delete(&bm->map.rle);
      break;
    case MAT:
      if ( bm->map.mat != NULL )
        matrix_delete((void **) bm->map.mat);
        // TODO Are we sure all matrices would have been allocated the way
        // that call requires?
      break;
  }
}

/*! \param `rle` and subfields must be `malloc()`ated pointers. */
void RleMap_delete(const struct RleMap *rle)
{
  for ( size_t l = 0 ; l < rle->nb_lines ; l++ )
    free(rle->lines[l].tokens);
  free(rle->lines);
}

/*! Delete a matrix which was allocated in one block, as is done
  in `matrix_new()`. */
void matrix_delete(void **a)
{
#if 0
  for ( int i = 0 ; i < m ; i++ )
    free(a[i]);
#endif
  free(a[0]);
  free(a);
}

void matrix_write(FILE *file, char **mat, size_t m, size_t n)
{
  for ( size_t i = 0 ; i < m ; i++ )
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
        };
        rle_flatten(&rle.tokens, bm->map.rle);
        life_rle_write(file, rle);
      }
      break;
    case MAT:
      break; // unimplemented
  }
}

/*! Split a stream of tokens (in the format understood by runlength.h
  functions) by lines. In the process, the token values are translated
  to binary values.

  Return 0 on success, 1 on system failure (full memory, etc.
  Check `errno`), 2 on incorrect token (should be one of the four
  from runlength.h). */
int align_tokens(struct RleMap *m, struct RleToken *rle)
{
  DArray lines, cur_tokens;
  size_t line_num = 0;
  da_init(&lines, sizeof(struct RleLine));
  da_init(&cur_tokens, sizeof(struct RleToken));
#define DESTROY_IF(x) \
  if ( x ) \
  { \
    da_destroy(&lines); \
    da_destroy(&cur_tokens); \
    return 1; \
  } // end of DESTROY_IF
#define NEW_LINE() DESTROY_IF( push_new_line(&lines, &cur_tokens, line_num) )
  for ( size_t i = 0 ; rle[i].value.char_ != END_RLE_TOKEN ; i++ )
  {
    int value = 1;
    switch ( rle[i].value.char_ )
    {
      case DEAD_RLE_TOKEN:
        value = 0;
      case ALIVE_RLE_TOKEN:
      {
        // value = 1 by default
        struct RleToken dest = {
          .value = { .int_ = value },
          .repeat = rle[i].repeat,
        };
        DESTROY_IF( NULL == da_push(&cur_tokens, &dest) );
        break;
      }
      case NEWLINE_RLE_TOKEN:
        NEW_LINE();
        line_num += rle[i].repeat;
        break;
      default:
        DESTROY_IF( 1 );
        return 2;
    }
  }
  NEW_LINE();
  DESTROY_IF( da_unpack(&lines, (void **) &m->lines, &m->nb_lines) );
  return 0;
#undef DESTROY_IF
#undef NEW_LINE
}

/*!
  Return 0 on success, 1 on system failure. */
int rle_flatten(struct RleToken **rle, struct RleMap rle_m)
{
  DArray rle_da;
  da_init(&rle_da, sizeof(struct RleToken));
#define DESTROY_IF(x) \
  if ( x ) \
  { \
    da_destroy(&rle_da); \
    return 1; \
  }
#define PUSH(x) DESTROY_IF( NULL == da_push(&rle_da, &x) )
  for ( size_t i = 0 ; i < rle_m.nb_lines ; i++ )
  {
    struct RleToken t_nl = {
      .value = { .char_ = NEWLINE_RLE_TOKEN },
      .repeat = rle_m.lines[i].line_num
              - (( i == 0 ) ? 0 : rle_m.lines[i-1].line_num)
    };
    if ( t_nl.repeat > 0 )
      PUSH(t_nl);
    for ( size_t j = 0 ; j < rle_m.lines[i].nb_tokens ; j++ )
    {
      struct RleToken src_ = rle_m.lines[i].tokens[j];
      struct RleToken src = {
        .value = {
          .int_ = src_.value.int_ ? ALIVE_RLE_TOKEN : DEAD_RLE_TOKEN },
        .repeat = src_.repeat,
      };
      PUSH(src);
    }
  }
  struct RleToken t_end = {
    .value = { .char_ = END_RLE_TOKEN },
    .repeat = 1
  };
  PUSH(t_end);
  DESTROY_IF( da_unpack(&rle_da, (void **) rle, NULL) )
  return 0;
#undef DESTROY_IF
#undef PUSH
}

/*! Auxiliary function for `align_tokens()`.

  Return 0 on success, 1 on failure.

  `token_line` is `da_unpack()`-ed but left unmodified in case of failure
  (so the caller may free it). */
int push_new_line(DArray *lines, DArray *token_line, size_t line_num)
{
  if ( !da_is_empty(token_line) )
  {
    struct RleLine new_line = { .line_num = line_num };
    if ( da_unpack(token_line, (void **) &new_line.tokens,
           &new_line.nb_tokens) )
      return 1;
    da_init(token_line, sizeof(struct RleToken));
    return ( NULL == da_push(lines, &new_line) );
  }
  else
    return 0;
}

