#include <stdio.h>
#include <stdlib.h>
#include "bigint.h"
#include "hashtbl.h"
#include "hashlife.h"

#define DEBUG

Quad *center(Hashtbl *htbl, Quad *quad[4], int d);

Quad *expand(Hashtbl *htbl, Quad *q, int d);

/**************************************************/

// Returns the configuration starting from q after 2^t steps
// Accepts a tree with depth d > t

Quad *fate(Hashtbl *htbl, Quad *q, int t)
{
  Quad *f = map_assoc(q->node.n.next, t);

  // quad->depth > t
  if ( f == NULL )
  {
    /* qs is the array of depth d-2 subtrees
         00 01 02 03
         10 11 12 13
         20 21 22 23
         30 31 32 33
       q1 represents step 2^(d-1)
         00 01 02
         10 11 12
         20 21 22 */

    Quad *qs[4][4], *q1[3][3], *nxt[4],
         **quad = q->node.n.sub;

    int i, j;
    
    const int d = q->depth;
    const int t_ = d == t + 1 ? t - 1 : t;

    for ( i = 0 ; i < 4 ; i++ )
      for ( j = 0 ; j < 4 ; j++ )
        qs[i][j] = quad[(i & 2) + (j >> 1)]->node.n.sub[2 * (i & 1) + (j & 1)];

    // we compute q1
    for ( i = 0 ; i < 3 ; i++ )
      for ( j = 0 ; j < 3 ; j++ )
      {
        Quad *tmp[4];
        int k;

        for ( k = 0 ; k < 4 ; k++ )
          tmp[k] = qs[i + (k >> 1)][j + (k & 1)];

        if ( d == t + 1 )
          q1[i][j] = fate(htbl, cons_quad(htbl, tmp, d - 1), t - 1);
        else
          q1[i][j] = center(htbl, tmp, d - 2);
      }

    // nxt=q->node.n.sub holds the quad tree pointer to step 2^d
    for ( i = 0 ; i < 2 ; i++ )
      for ( j = 0 ; j < 2 ; j++ )
      {
        Quad *tmp[4];
        int k;

        for ( k = 0 ; k < 4 ; k++ )
          tmp[k] = q1[i + (k >> 1)][j + (k & 1)];

        Quad *tmpq = cons_quad(htbl, tmp, d - 1);

        nxt[2 * i + j] = fate(htbl, tmpq, t_);
      }

    f = cons_quad(htbl, nxt, d-1);

    map_add(&q->node.n.next, t, f);
  }

  return f;
}

// Computes the configuration starting from q after bi steps
// The returned quadtree will represent a greater zone than the original one
// to enable keeping track of effects outside.
// The map is assumed to be infinite in all directions, dead cells by default
// so that extension is possible.
// 
//  0 1
//  2 3
// 
// The original zone is located at the top left of 3,
// (2^shift_e) is the width and height of 0, 1, 2 (and 3)

Quad *destiny(Hashtbl *htbl, Quad *q, const BigInt *bi, int *shift_e)
{
  int d = q->depth;
  int len = bi_log2(bi);

  // Increase the size of the quad tree so that the center square
  // can contain all the effects of the starting configuration
  while ( len > d + 1 )
  {
    Quad *ds = dead_space(htbl, d);
    Quad *quad[4] = {q, ds, ds, ds};

    q = cons_quad(htbl, quad, ++d);
  }

  Quad *ds = dead_space(htbl, d);
  Quad *quad[4] = {ds, ds, ds, q};

  q = cons_quad(htbl, quad, ++d);

  *shift_e = d;

  ds = dead_space(htbl, d);
  Quad *quad_[4] = {q, ds, ds, ds};

  q = cons_quad(htbl, quad_, ++d);

  // Progress by powers of two
  for ( len-- ; len >= 0 ; len-- )
  {
    if ( bi_digit(bi, len) )
      q = fate(htbl, expand(htbl, q, d + 1), len);
  }

  return q;
}

// From the quad
//
//  0 1
//  2 3
//
// returns the quad for
//
//  . . . .
//  . 0 1 .
//  . 2 3 .
//  . . . .
//
// where . is all zeroes

Quad *expand(Hashtbl *htbl, Quad *q, int d)
{
  Quad *ds = dead_space(htbl, d - 2);
  Quad *quad[4] = {ds, ds, ds, ds};

  Quad *quad2[4];

  int i;

  for ( i = 0 ; i < 4 ; i++ )
  {
    quad[3-i] = q->node.n.sub[i];
    quad2[i] = cons_quad(htbl, quad, d-1);
    quad[3-i] = ds;
  }

  return cons_quad(htbl, quad2, d);
}

// Inverse of expand,
// accepts the four subquads of
//
//  * * * *
//  * 0 1 *
//  * 2 3 *
//  * * * *
//
// and returns the middle square,
// in particular does not require that the outer ring is all dead cells
// although it is the case when used in destiny()

Quad *center(Hashtbl *htbl, Quad *quad[4], int d)
{
  if ( d == 0 )
  {
    int i, l = 0;
    for ( i = 0 ; i < 4 ; i++ )
      l |= quad[i]->node.l.map[3-i] << (3-i);

    return leaf(l);
  }
  else
  {
    int i;
    for ( i = 0 ; i < 4 ; i++ )
      quad[i] = quad[i]->node.n.sub[3-i];

    return cons_quad(htbl, quad, d);
  }
}

