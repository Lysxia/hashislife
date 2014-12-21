#ifndef RUNLENGTH_H
#define RUNLENGTH_H

/* Run length encoding */

#include "darray.h"
#include "definitions.h"
#include "parsers.h"

#define END_RLE_TOKEN '!'
#define DEAD_RLE_TOKEN 'b'
#define ALIVE_RLE_TOKEN 'o'
#define NEWLINE_RLE_TOKEN '$'

struct RleToken
{
  int value;
  int repeat;
};

struct TokenParser
{
  FILE *file;
  char *buff;
  int   i;
};

struct TokenPrinter
{
  FILE *file;
  int   line_length;
};

struct TokenParser tp_new(FILE *file);
int tp_regenerate(struct TokenParser *tp);
struct RleToken rle_token(struct TokenParser *tp);

void rle_print(struct TokenPrinter *, struct RleToken);

struct RleToken *read_rle(FILE *file);
void            write_rle(FILE *file, struct RleToken *rle);

#endif
