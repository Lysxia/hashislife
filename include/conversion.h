#ifndef CONVERSION_H
#define CONVERSION_H

#include "bigint.h"
#include "bitmaps.h"
#include "hashtbl.h"

typedef union UMatrix
{
  char **char_;
  const BigInt ***bi_;
} UMatrix;

#if 0
// Assume the remaining cells are dead cells
// A more general setting could be imagined... Toric prgrph...
Quad *prgrph_to_quad(Hashtbl *htbl, Prgrph p);
#endif

Quad *rle_to_quad(Hashtbl *htbl, const struct RleMap *rle);

// Draw the prgrph described by q at the specified location
UMatrix quad_to_matrix(
  Quad *q,
  int zoom, //! >= 0
  const BigInt *mmin,
  int mlen,
  const BigInt *nmin,
  int nlen);

struct FramePositionBig {
  int m_min;
  const BigInt *min;
  int len;
};

struct FramePosition {
  int m_min;
  int min;
  int len;
};

void simple_quad_to_matrix(
  UMatrix p,
  Quad *q,
  const int tree_h, // < 31
  const struct FramePosition m,
  const struct FramePosition n);

void cropping_quad_to_matrix(
  UMatrix p,
  Quad *q,
  const int tree_h, // < 31
  struct FramePosition m,
  struct FramePosition n);

void quad_to_matrix_(
  UMatrix p,
  Quad *q,
  int tree_h,
  struct FramePositionBig m,
  struct FramePositionBig n);

void free_um_char(UMatrix um, int m);
void free_um_bi(UMatrix um, int m);

#endif
