#ifndef CONVERSION_AUX_H
#define CONVERSION_AUX_H

#include "quad.h"

/* Mirroring the basic RLE structures */ 
struct QRleToken
{
  Quad *value;
  int   repeat;
};

struct QRleLine
{
  struct QRleToken *tokens;
  int nb_tokens;
  int line_num;
};

struct QRleMap
{
  struct QRleLine *lines;
  int nb_lines;
};

struct QRleMap prgrph_to_qrle(Prgrph p);
struct QRleMap RleMap_to_QRleMap(RleMap *rle_m);

Quad *condense(Hashtbl *htbl, struct QRleMap qrle_m);

struct QRleLine fuse_RleLines(struct RleLine line[2]);
#endif
