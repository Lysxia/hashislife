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
  if ( q->alive )
    return q->alive;
  else
  {
    q->alive = malloc(sizeof(BigInt));
    if ( NULL == q->alive )
    {
      perror("cell_count()");
      exit(1);
    }
    if ( q->depth > 0 )
    {
      BigInt tmp[2];
      int i;
      for ( i = 0 ; i < 2 ; i++ )
        bi_add(&tmp[i],
          cell_count_(q->node.n.sub[2*i]),
          cell_count_(q->node.n.sub[2*i+1]));

      bi_add(q->alive, &tmp[0], &tmp[1]);

      bi_clear(&tmp[0]);
      bi_clear(&tmp[1]);
    }
    else // q->depth == 0
    {
      int i, k = 0;
      for ( i = 0 ; i < 4 ; i++ )
        k += q->node.l.map[i];

      bi_simple(q->alive, k);
    }
    return q->alive;
  }
}

