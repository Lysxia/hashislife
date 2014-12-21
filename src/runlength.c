#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "runlength.h"
#include "definitions.h"
#include "darray.h"
#include "parsers.h"

#define RLE_LINE_LENGTH 100

int tp_regenerate(struct TokenParser *tp)
{
  tp->i = 0;
  return ( tp->file != NULL
        && fgets(tp->buff, RLE_LINE_LENGTH, tp->file) != NULL );
}

struct RleToken rle_token(struct TokenParser *tp)
{
  char buff[10] = {'\0'};
  int j;
  struct RleToken t = {
    .value = 0,
    .repeat = -1 };

  while ( tp->buff[tp->i] == '\0' || isspace(tp->buff[tp->i]) )
  {
    tp->i += strspn(tp->buff + tp->i, " \t\n"); // Skip spaces
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
      t.value = tp->buff[tp->i];
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
    .buff = calloc(RLE_LINE_LENGTH, sizeof(char)),
    .i = 0
  };
}

struct RleToken *read_rle(FILE *file)
{
  char buff[RLE_LINE_LENGTH];
  int linum = 0;

  do
  {
    linum++;
    if ( fgets(buff, RLE_LINE_LENGTH, file) == NULL )
    {
      fprintf(stderr, "read_rle(): Error on input\n");
      return NULL;
    }
  }
  while ( buff[0] == '#' );

  char s[22];
  int x, y;
  rule r;
  switch ( sscanf(buff, "x = %d, y = %d, rule = %21s ", &x, &y, s) )
  {
    case 2:
      r = 0;
      break;
    case 3:
      if ( (r = parse_rule(s)) != (rule) (-1) )
        break;
    case EOF:
    case 1:
    default:
      perror("read_rle(): Syntax error.");
      fprintf(stderr,"Line %d: %s\n", linum, s);
      exit(3);
      return NULL;
  }

  Darray *rle = da_new(sizeof(struct RleToken));
  if ( rle == NULL )
  {
    perror("read_rle()");
    exit(1);
  }

  struct RleToken t;
  struct TokenParser tp = tp_new(file);
  do {
    t = rle_token(&tp);
    if ( t.repeat < 0 )
    {
      free(da_unpack(rle, NULL));
      perror("read_rle(): Syntax error.");
      exit(3);
    }
    da_push(rle, &t);
  } while ( t.value != END_RLE_TOKEN );

  return da_unpack(rle, NULL);
}

#define MIN(a,b) (((a) < (b)) ? a : b)

void write_rle(FILE *file, struct RleToken *rle)
{
  int i;
  struct TokenPrinter tp = {
    .file = file,
    .line_length = 0
  };
  for ( i = 0 ; rle[i].value != END_RLE_TOKEN ; i++ )
  {
    rle_print(&tp, rle[i]);
  }
  struct RleToken t_end = {
    .value = END_RLE_TOKEN,
    .repeat = 1
  };
  rle_print(&tp, t_end);
  fflush(file);
}

#define MAX_COLS 79
void rle_print(struct TokenPrinter *tp, struct RleToken t)
{
  char a[12];

  int written;
  if ( t.repeat == 1 ) {
    a[0] = t.value;
    a[1] = '\0';
    written = 1;
  } else {
    written = itoa(a, t.repeat, 10);
    a[written] = t.value;
    a[written+1] = '\0';
    written++;
  }

  if ( (tp->line_length += written) > MAX_COLS )
  {
    fputc('\n', tp->file);
    tp->line_length = 0;
  }
  
  fputs(a, tp->file);
}

