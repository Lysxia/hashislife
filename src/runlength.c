#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "runlength.h"
#include "definitions.h"
#include "darray.h"

#define Rle_LINE_LENGTH 100

int life_rle_read(struct LifeRle *rle, FILE *file)
{
  life_rle_in = file;
  return life_rle_lex(rle);
}

#define MIN(a,b) (((a) < (b)) ? a : b)

void life_rle_write(FILE *file, struct LifeRle rle)
{
  struct TokenWriter tw = {
    .file = file,
    .line_length = 0
  };
  /* Header */
  fputs("x = ", file);
  // This buffer should be long enough to hold a decimal representation of x
  // in base 10.
  char buff_x[bi_log2(&rle.x)];
  bi_to_string(buff_x, &rle.x, 10);
  fputs(buff_x, file);
  fputs(", y = ", file);
  char buff_y[bi_log2(&rle.y)];
  bi_to_string(buff_y, &rle.y, 10);
  fputs(buff_y, file);
  if ( rle.r != 0 )
  {
    // unimplemented
  }
  fputc('\n', file);
  write_tokens(&tw, rle.tokens, rle.nb_tokens);
  fputc('\n', file);
  fflush(file);
}
#undef MIN

void write_tokens(struct TokenWriter *tw, struct RleToken *rle, size_t n)
{
  for ( size_t i = 0 ; i < n ; i++ )
    write_one_token(tw, rle[i]);
  struct RleToken t_end = { .value = { .char_ = END_RLE_TOKEN } };
  bi_simple(&t_end.repeat, 1);
  write_one_token(tw, t_end);
  bi_clear(&t_end.repeat); // NOP but clean
}

#define MAX_COLS 79
void write_one_token(struct TokenWriter *tw, struct RleToken t)
{
  BigInt one; bi_simple(&one, 1);
  const int compare_one = bi_compare(&t.repeat, &one);
  assert( 0 <= compare_one );

  char a[2 + bi_log2(&t.repeat)];
  size_t written;
  if ( 0 == compare_one ) {
    a[0] = t.value.char_;
    a[1] = '\0';
    written = 1;
  } else {
    written = bi_to_string(a, &t.repeat, 10) + 1;
    a[written-1] = t.value.char_;
    a[written] = '\0';
  }

  if ( MAX_COLS < (tw->line_length += written) )
  {
    fputc('\n', tw->file);
    tw->line_length = written;
  }
  
  fputs(a, tw->file);
}

void *push_token(DArray *rle_da, union Tokenizable value, const BigInt *repeat)
{
  struct RleToken t =
  {
    .value = value,
    .repeat = *repeat,
  };
  return da_push(rle_da, &t);
}

int life_rle_unpack(struct LifeRle *rle, DArray *rle_da)
{
  return da_unpack(rle_da, (void **) &rle->tokens, &rle->nb_tokens);
}

