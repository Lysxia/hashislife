#include <assert.h>
#include "hashtbl.h"
#include "hashtbl_aux.h"

/*!
  From the quad
 
      0 1
      2 3
 
  returns the quad for
 
      . . . .
      . 0 1 .
      . 2 3 .
      . . . .
 
  where `.` is all zeroes
*/
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

/*! Inverse of expand(),
  accepts the four subquads of
 
 *    * * * *
 *    * 0 1 *
 *    * 2 3 *
 *    * * * *
 
  and returns the middle square,
  in particular does not require that the outer ring is all dead cells
  although it is the case when used in destiny()
*/
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

void flatten(Quad *dest[4][4], Quad *src)
{
  int i, j;
  for ( i = 0 ; i < 4 ; i++ )
    for ( j = 0 ; j < 4 ; j++ )
      dest[i][j] = src->node.n.sub[(i & 2) + (j >> 1)]
                      ->node.n.sub[2 * (i & 1) + (j & 1)];
}

/*! Prerequisite : the four subtrees were computed and hashed.

  Use this function to construct new nodes.
*/
Quad *cons_quad(
  Hashtbl *htbl, //!< Current hashtable
  Quad *quad[4], //!< Array of subtrees
  int d)         //!< Depth of current node
{
  assert(quad[0]->depth == quad[1]->depth
      && quad[0]->depth == quad[2]->depth
      && quad[0]->depth == quad[3]->depth
      && quad[0]->depth == d-1);

  int h = hash(quad);

  Quad *q = hashtbl_find(htbl, h, quad);

  if ( q ) // The requested node has been memoized already
    return q;
  else
  {
    QuadList *ql = quadlist_alloc(htbl);

    ql->head.depth = d;
    ql->head.alive = NULL;
    ql->head.node.n.skip = NULL;
    ql->head.node.n.short_skip = NULL;

    int i;
    for ( i = 0 ; i < 4 ; i++ )
      ql->head.node.n.sub[i] = quad[i];

    hashtbl_add(htbl, h, ql);

    return &ql->head;
  }
}

