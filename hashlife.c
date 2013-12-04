#include <stdio.h>
#include <stdlib.h>
#include "hashtbl.h"
#include "hashlife.h"

#define DEBUG

Quad *center(Hashtbl *htbl, Quad *quad[4], int d);

Quad *fate(Hashtbl *htbl, Quad *q, int t)
{
  Quad *f = map_assoc(q->node.n.next, t);
  // quad->depth > t
  if (f == NULL)
  {
    // Unwrap quad tree
    Quad *qs[4][4], *q1[3][3], *nxt[4],
         **quad = q->node.n.sub;

    int i, j, d = q->depth;

    /* qs is the array of depth d-2 subtrees
         00 01 02 03
         10 11 12 13
         20 21 22 23
         30 31 32 33
       q1 represents step 2^(d-1)
         00 01 02
         10 11 12
         20 21 22 */

    const int t_ = d == t + 1 ? t - 1 : t;

    for (i = 0 ; i < 4 ; i++)
      for (j = 0 ; j < 4 ; j++)
        qs[2 * (i / 2) + j / 2][2 * (i % 2) + j % 2] = quad[i]->node.n.sub[j];

    // we compute q1
    for (i = 0 ; i < 3 ; i++)
      for (j = 0 ; j < 3 ; j++)
      {
        Quad *tmp[4];
        int k;

        for (k = 0 ; k < 4 ; k++)
          tmp[k] = qs[i + (k >> 1)][j + (k & 1)];

        if (d == t+1)
          q1[i][j] = fate(htbl, cons_quad(htbl, tmp, d-1), t-1);
        else
          q1[i][j] = center(htbl, tmp, d-2);
      }

    // nxt=q->node.n.sub holds the quad tree pointer to step 2^d
    for (i = 0 ; i < 2 ; i++)
      for (j = 0 ; j < 2 ; j++)
      {
        Quad *tmp[4];
        int k;

        for (k = 0 ; k < 4 ; k++)
          tmp[k] = q1[i + (k >> 1)][j + (k & 1)];

        nxt[2 * i + j] = fate(htbl, cons_quad(htbl, tmp, d - 1), t_);
      }

    f = cons_quad(htbl, nxt, d-1);

    map_add(&q->node.n.next, t, f);
  }

  return f;
}

Quad *center(Hashtbl *htbl, Quad *quad[4], int d)
{
  if (d == 1)
  {
    int i, l = 0;
    for (i = 0 ; i < 4 ; i++)
      l |= quad[i]->node.l.map[3-i] << (3 - i);

    return leaf(l);
  }
  else
  {
    int i;
    for (i = 0 ; i < 4 ; i++)
      quad[i] = quad[i]->node.n.sub[3-i];

    return cons_quad(htbl, quad, d);
  }
}
