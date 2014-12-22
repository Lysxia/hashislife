#ifndef HASHLIFE_H
#define HASHLIFE_H
#include "bigint.h"

typedef struct Task Task;

struct Task {
  int           id;
  const BigInt *steps;
  Chunks        chunks;
};

//! Return the `.skip` or `.short_skip` field of the given quadtree node
Quad *skip(Hashtbl *htbl, Quad *q, Task *task);

Quad *destiny(Hashtbl *htbl, Quad *q, Task *task);

#endif
