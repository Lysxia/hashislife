#include <stdlib.h>
#include <stdio.h>
#include "lifecount.h"
#include "bigint.h"
#include "hashtbl.h"

const BigInt *cell_count_(Quad *);

const BigInt *cell_count(Quad *q)
{
  return cell_count_(q);
}

const BigInt *cell_count_(Quad *q)
{
  if ( q->cell_count )
    return q->cell_count;
  else if ( q->depth > 0 )
  {
    BigInt *tmp[2];
    int i;
    for ( i = 0 ; i < 2 ; i++ )
        tmp[i] = bi_add(cell_count_(q->node.n.sub[2*i]),
                        cell_count_(q->node.n.sub[2*i+1]));
    q->cell_count = bi_add(tmp[0], tmp[1]);

    bi_free(tmp[0]);
    bi_free(tmp[1]);

    return q->cell_count;
  }
  else // q->depth == 0
  {
    int i, k = 0;
    for ( i = 0 ; i < 4 ; i++ )
      k += q->node.l.map[i];

    return q->cell_count = bi_from_int(k);
  }
}

