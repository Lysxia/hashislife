#ifndef CONVERSION_H
#define CONVERSION_H

#include "bigint.h"
#include "prgrph.h"
#include "hashtbl.h"

typedef union UMatrix
{
  char **um_char;
  const BigInt ***um_bi;
} UMatrix;

// Assume the remaining cells are dead cells 
// A more general setting could be imagined... Toric prgrph...
Quad *prgrph_to_quad(Hashtbl *htbl, Prgrph prgrph);

// Draw the prgrph described by q at the specified location
UMatrix quad_to_prgrph(BigInt *mmin, BigInt *nmin,
                          int mlen, int nlen,
                          int height, Quad *q);

Prgrph bi_mat_to_prgrph(const BigInt ***bm, int m, int n, int height);

#endif
