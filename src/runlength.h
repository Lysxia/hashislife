#ifndef RUNLENGTH_H
#define RUNLENGTH_H

/* Run length encoding */

#include <stdlib.h>
#include <stdio.h>

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

struct TokenWriter
{
  FILE *file;
  int   line_length;
};

struct RLE
{
  int x;
  int y;
  rule r;
  struct RleToken *tokens;
};

struct TokenParser tp_new(FILE *file);
int tp_regenerate(struct TokenParser *tp);
struct RleToken rle_token(struct TokenParser *tp);

void write_tokens(struct TokenWriter *, struct RleToken *);
void write_one_token(struct TokenWriter *, struct RleToken);

struct RLE read_rle(FILE *file);
void       write_rle(FILE *file, struct RLE rle);

#endif
