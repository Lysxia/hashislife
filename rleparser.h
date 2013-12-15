#ifndef RLEPARSER_H
#define RLEPARSER_H

#include "darray.h"
#include "parsers.h"

typedef struct Rle_line
{
  Darray line_rle;
  int line_num;
} Rle_line;

void bm_free(BitMap *map);

int rle_token(FILE *new_file, char *tag);

BitMap *read_rle(FILE *file);
void print_rle(Darray *rle, FILE *file);

#endif
