#ifndef HASHLIFE_H
#define HASHLIFE_H

#include "bigint.h"

Quad *fate(Hashtbl *htbl, Quad *q, int t);

Quad *destiny(
  Hashtbl *htbl,
  Quad *q,
  const BigInt *bi,
  int *shift_e);

#endif
