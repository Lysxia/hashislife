#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "runlength.h"
#include "definitions.h"
#include "darray.h"
#include "parsers.h"

#define Rle_LINE_LENGTH 100

int tp_regenerate(struct TokenParser *tp)
{
  if ( tp->file != NULL
    && fgets(tp->buff, Rle_LINE_LENGTH, tp->file) != NULL )
  {
    tp->i = 0;
    return 1;
  }
  else {
    return 0;
  }
}

struct RleToken life_rle_token(struct TokenParser *tp)
{
  char buff[10] = {'\0'};
  int j;
  struct RleToken t =
  {
    .value.char_ = 0,
    .repeat = -1
  };

  while ( tp->buff[tp->i] == '\0' || isspace(tp->buff[tp->i]) )
  {
    // Skip spaces
    tp->i += strspn(tp->buff + tp->i, "\x20\x0c\x0a\x0d\x09\x0b");
    if ( tp->buff[tp->i] == '\0' && !tp_regenerate(tp) )
      return t; // EOF
  }
  j = strspn(tp->buff + tp->i, "0123456789");
  if ( j > 10 )
    return t; // integer too large
  memcpy(buff, tp->buff + tp->i, j);
  tp->i += j;
  int z = ( j == 0 ) ? 1 : atoi(buff);
  switch ( tp->buff[tp->i] )
  {
    case END_RLE_TOKEN:
    case DEAD_RLE_TOKEN:
    case ALIVE_RLE_TOKEN:
    case NEWLINE_RLE_TOKEN:
      t.value.char_ = tp->buff[tp->i];
      t.repeat = z;
      break;
  }
  tp->i++;
  return t;
}

struct TokenParser tp_new(FILE *file)
{
  return (struct TokenParser) {
    .file = file,
    .buff = calloc(Rle_LINE_LENGTH, sizeof(char)),
    .i = 0
  };
}

struct LifeRle life_rle_read(FILE *file)
{
  char buff[Rle_LINE_LENGTH];
  int linum = 0;
  const struct LifeRle fail_rle = { .tokens = NULL, .r = -1 };

  do
  {
    linum++;
    if ( fgets(buff, Rle_LINE_LENGTH, file) == NULL )
    {
      perror("read_rle(): Error on input");
      return fail_rle;
    }
  }
  while ( buff[0] == '#' );

  char s[22];
  struct LifeRle rle;
  switch ( sscanf(buff, "x = %d, y = %d, rule = %21s ", &rle.x, &rle.y, s) )
  {
    case 2:
      rle.r = 0;
      break;
    case 3:
      if ( (rle.r = parse_rule(s)) != (rule) (-1) )
        break;
    case EOF:
    case 1:
    default:
      perror("read_rle(): Syntax error.");
      fprintf(stderr,"Line %d: %s\n", linum, s);
      return fail_rle;
  }

  Darray *rle_da = da_new(sizeof(struct RleToken));
  if ( rle_da == NULL )
  {
    perror("read_rle()");
    exit(1);
  }

  struct RleToken t;
  struct TokenParser tp = tp_new(file);
  do {
    t = life_rle_token(&tp);
    if ( t.repeat < 0 )
    {
      free(da_unpack(rle_da, NULL));
      perror("read_rle(): Syntax error.");
      exit(3);
    }
    da_push(rle_da, &t);
  } while ( t.value.char_ != END_RLE_TOKEN );

  rle.tokens = da_unpack(rle_da, NULL);
  return rle;
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
#undef MAX_COLS

