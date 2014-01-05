#ifndef RLEPARSER_H
#define RLEPARSER_H

#include "darray.h"
#include "bitmaps.h"
#include "parsers.h"

#define DEAD_RLE_TOKEN 'b'
#define ALIVE_RLE_TOKEN 'o'
#define NEWLINE_RLE_TOKEN '$'

int rle_token(FILE *new_file, char *tag);

BitMap *read_rle(FILE *file);
void write_rle(Darray *rle, FILE *file);

#endif
