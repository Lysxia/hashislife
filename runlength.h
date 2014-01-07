#ifndef RUNLENGTH_H
#define RUNLENGTH_H

/* Run length encoding */

#include "darray.h"
#include "definitions.h"
#include "parsers.h"

#define DEAD_RLE_TOKEN 'b'
#define ALIVE_RLE_TOKEN 'o'
#define NEWLINE_RLE_TOKEN '$'

struct Rle_line
{
  int *line;
  int line_length;
  int line_num;
};

typedef struct Rle
{
  struct Rle_line *rle_lines;
  int rle_lines_c;
  struct {
    int rle_x, rle_y;
    rule rle_r;
  } rle_meta;
} Rle;

Rle *read_rle(FILE *file);
void write_rle(Rle *rle, FILE *file);

#endif
