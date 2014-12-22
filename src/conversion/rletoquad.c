#include "conversion.h"
#include "conversion_aux.h"
#include "hashtbl.h"

Quad *rle_to_quad(Hashtbl *htbl, struct RleMap *rle_m)
{
  return condense(htbl, fuse_adjacent_lines(rle_m, token_leaf));
}

Quad *condense(Hashtbl *htbl, struct RleMap q_rle_m)
{
}

