#ifndef CONVERSION_H
#define CONVERSION_H

#include "bigint.h"
#include "bitmaps.h"
#include "prgrph.h"
#include "hashtbl.h"

typedef union UMatrix
{
  char **um_char;
  const BigInt ***um_bi;
} UMatrix;

// Assume the remaining cells are dead cells 
// A more general setting could be imagined... Toric prgrph...
Quad *prgrph_to_quad(Hashtbl *htbl, Prgrph p);
Quad *rle_to_quad(Hashtbl *htbl, RleMap *rle);

// Draw the prgrph described by q at the specified location
UMatrix quad_to_matrix(
  BigInt *mmin,
  BigInt *nmin,
  int mlen,
  int nlen,
  int height,
  Quad *q);

struct FrameIndices {
  int m_min;
  int min;
  int len;
};

void simple_quad_to_matrix(
  UMatrix p,
  Quad *q,
  const int tree_h, // < 31
  const struct FrameIndices m,
  const struct FrameIndices n);

void cropping_quad_to_matrix(
  UMatrix p,
  Quad *q,
  const int tree_h, // < 31
  struct FrameIndices m,
  struct FrameIndices n);

Prgrph bi_mat_to_prgrph(
  const BigInt ***bm,
  int m,
  int n,
  int height);

void free_um_char(UMatrix um, int m);
void free_um_bi(UMatrix um, int m);

#endif
