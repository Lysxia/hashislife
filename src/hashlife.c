#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "bigint.h"
#include "hashtbl.h"
#include "hashlife.h"

/*! */
Quad *skip(
  Hashtbl *htbl,
  Quad    *q,    //!< Not a leaf
  Task    *task) //!< Pass `NULL` to compute `.skip`, `.short_skip` otherwise
{
  const int d = q->depth;
  assert( 0 < d );

  const int long_skip = NULL == task;
  // Macro because this is ill defined when task == NULL

  /* The recursion stops at depth 1 in the worst case */
  if ( long_skip && NULL != q->node.n.skip )
    return q->node.n.skip;
  if ( q->depth == 1 )
    // long_skip == false, otherwise the previous branch would have been taken
    // Depth 1 nodes will always have their short_skip field set to NULL.
    // In that case they always return the center area.
    return center(htbl, q->node.n.sub, 0);
  #define TASK_DONE !long_skip && NULL != q->node.n.short_skip && task->id == q->node.n.short_skip->task_id
  else if ( TASK_DONE )
    return q->node.n.short_skip->head;

  const int half_skip = long_skip || bi_digit(task->steps, d-2);
  /* qs is the array of depth d-2 subtrees:

      00 01 02 03
      10 11 12 13
      20 21 22 23
      30 31 32 33 */
  Quad *qs[4][4];
  flatten(qs, q);
  
  /* q1 contains the progress after 2^(d-1) steps at the centers of
     2x2 subblocks of qs:

      00 01 02
      10 11 12
      20 21 22 */
  Quad *q1[3][3];
  for ( int i = 0 ; i < 3 ; i++ )
    for ( int j = 0 ; j < 3 ; j++ )
    {
      Quad *cur_quad[4];

      int k;
      for ( k = 0 ; k < 4 ; k++ )
        cur_quad[k] = qs[i + (k >> 1)][j + (k & 1)];

      if ( half_skip )
        q1[i][j] = skip(htbl, cons_quad(htbl, cur_quad, d - 1), NULL);
      else
        q1[i][j] = center(htbl, cur_quad, d-2);
    }

  /* Holds the subtrees after step 2^d */
  Quad *nxt[4];
  for ( int i = 0 ; i < 2 ; i++ )
    for ( int j = 0 ; j < 2 ; j++ )
    {
      Quad *cur_quad[4];

      for ( int k = 0 ; k < 4 ; k++ )
        cur_quad[k] = q1[i + (k >> 1)][j + (k & 1)];

      nxt[2 * i + j] = skip(htbl, cons_quad(htbl, cur_quad, d - 1), task);
    }

  Quad *q_out = cons_quad(htbl, nxt, d-1);

  if ( long_skip )
    q->node.n.skip = q_out;
  else {
    QuadTaskList *qtl = chunks_alloc(task->chunks);
    qtl->task_id = task->id;
    qtl->head = q_out;
    qtl->tail = q->node.n.short_skip;
    q->node.n.short_skip = qtl;
  }

  return q_out;
}

/*! Computes the configuration starting from q after bi steps

  The returned quadtree will represent a greater zone than the original one
  to enable keeping track of effects outside.
  The view of the original area can be recovered by successive calls to
  `center()` (until the original depth).

  The map is assumed to be infinite in all directions,
  extended with dead cells.
*/
Quad *destiny(
  Hashtbl *htbl,
  Quad    *q, //!< Not a leaf
  Task    *task)
{
  const int len = bi_log2(task->steps);

  /* Increase the size of the quad tree so that the center square
    can contain all the effects of the starting configuration */
  do
  {
    q = expand(htbl, q, q->depth+1);
  }
  while ( q->depth < len + 2 );

  return skip(htbl, q, task);
}

