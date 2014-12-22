#include "conversion.h"
#include "conversion_aux.h"
#include "hashtbl.h"

Quad *rle_to_quad(Hashtbl *htbl, RleMap *rle_m)
{
  return condense(htbl, RleMap_to_QRleMap(rle_m));
}

Quad *condense(Hashtbl *htbl, struct QRleMap qrle_m)
{
}

