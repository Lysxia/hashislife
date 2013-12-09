#include <stdlib.h>

#include "bigint.h"

void life_count_add(Quad *q, BigInt c);

BigInt life_count(Quad *q)
{
  BigInt s = bi_zero;

  life_count_add(q, s);

  return s;
}

void life_count_add(Quad *q, BigInt c)
{
}

void life_matrix(BigInt **mat, int corner, int depth, Quad *q)
{
}
