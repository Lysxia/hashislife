#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "runlength.h"
#include "definitions.h"
#include "darray.h"
#include "parsers.h"

#define Rle_LINE_LENGTH 100

struct LifeRle life_rle_read(FILE *file)
{
  life_rle_in = file;
  return life_rle_lex();
}

#define MIN(a,b) (((a) < (b)) ? a : b)

void life_rle_write(FILE *file, struct LifeRle rle)
{
  struct TokenWriter tw = {
    .file = file,
    .line_length = 0
  };
  char buff[Rle_LINE_LENGTH];
  snprintf(buff, Rle_LINE_LENGTH, "x = %d, y = %d", rle.x, rle.y);
  fputs(buff, file);
  if ( rle.r != 0 )
  {
    // unimplemented
  }
  fputc('\n', file);
  write_tokens(&tw, rle.tokens);
  fputc('\n', file);
  fflush(file);
}
#undef MIN

void write_tokens(struct TokenWriter *tw, struct RleToken *rle)
{
  int i;
  for ( i = 0 ; rle[i].value.char_ != END_RLE_TOKEN ; i++ )
  {
    write_one_token(tw, rle[i]);
  }
  struct RleToken t_end =
  {
    .value = { .char_ = END_RLE_TOKEN },
    .repeat = 1
  };
  write_one_token(tw, t_end);
}

#define MAX_COLS 79
void write_one_token(struct TokenWriter *tw, struct RleToken t)
{
  if ( t.repeat == 0 )
    return;

  char a[12];

  int written;
  if ( t.repeat == 1 ) {
    a[0] = t.value.char_;
    a[1] = '\0';
    written = 1;
  } else {
    written = itoa(a, t.repeat, 10);
    a[written] = t.value.char_;
    a[written+1] = '\0';
    written++;
  }

  if ( (tw->line_length += written) > MAX_COLS )
  {
    fputc('\n', tw->file);
    tw->line_length = 0;
  }
  
  fputs(a, tw->file);
}

void push_token(DArray *rle_da, union Tokenizable value, int repeat)
{
  struct RleToken t =
  {
    .value = value,
    .repeat = repeat,
  };
  da_push(rle_da, &t);
}

