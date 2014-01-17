#include <stdlib.h>
#include <stdio.h>
#include "lifecount.h"
#include "bigint.h"
#include "hashtbl.h"

BigInt *life_count(Quad *q)
{
  return q->cell_count;
}

