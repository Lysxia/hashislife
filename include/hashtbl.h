#ifndef HASHTBL_H
#define HASHTBL_H
/* Quadtree hashconsing */

#include "bigint.h"
#include "chunks.h"
#include "definitions.h"
#include "quad.h"

/*! \defgroup hashtbl Hashtables */
/*!@{*/
typedef struct Hashtbl Hashtbl;
typedef struct QuadList QuadList;
typedef struct QuadBlock QuadBlock;

//! Hashtable of quadtrees
/*! */
struct Hashtbl
{
  rule       rule;
  int        size;      //!< Number of "buckets"
  QuadList **tbl;       //!< Hashtable contents

  int        dead_size; //!< Current length of dead areas array
  Quad     **dead_quad; //!< Array of quadtrees representing dead areas

  int        count;     //!< Number of created and inserted elements
  Chunks     chunks;    //!< Managed memory store
};

//! List of quadtrees
struct QuadList
{
  Quad      head; //!< Note that the head element is inlined as a field.
  QuadList *tail;
};

/* Create and delete */
Hashtbl *hashtbl_new(rule r); //!< Create hashtbl
void     hashtbl_delete(Hashtbl*); //!< Deallocate hashtbl

/* Globally initialize and quit */
void hashlife_init(void); //!< Global initializer
void hashlife_cleanup(void); //!< Clear hashlife resources

/* Simple blocks */
Quad *leaf(int k); //!< `int` to leaf
Quad *dead_space(Hashtbl *htbl, int d);
  //!< Returns an area of dead cells at depth `d`

//! Debugging
/*! Use `quad_to_matrix()` */
void print_quad(Quad*);
void hashtbl_stat(Hashtbl*);
int  depth1_skip(Hashtbl*, int[4]);
/*!@}*/

/*! \addtogroup quad */
/*!@{*/
Quad *cons_quad(
  Hashtbl *htbl,
  Quad *quad[4],
  int d); //!< Hashcons operation

Quad *center(Hashtbl *htbl, Quad *quad[4], int d); //!< Center area
Quad *expand(Hashtbl *htbl, Quad *q, int d);
  //!< Construct the area whose center is `q` and is filled with 0 elsewhere
void flatten(Quad *dest[4][4], Quad *src);
  //!< Fill a 4x4 array with subsubtrees
/*!@}*/
#endif
