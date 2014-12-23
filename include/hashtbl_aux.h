#ifndef HASHTBL_AUX_H
#define HASHTBL_AUX_H
/* Auxiliary definitions for hashtable manipulation */

#include "hashtbl.h"
#include "quad.h"

/*! \defgroup hashtbl_aux Auxiliary definitions (hashtables) */
/*!@{*/
QuadList *quadlist_alloc(Hashtbl *htbl);

//! Create depth 1 node.
void quad_d1(Hashtbl *htbl, Quad *quad[4], rule r);

int   hash(Quad*[4]); //!< Quadtree hash function
Quad *hashtbl_find(Hashtbl *htbl, int h, Quad *key[4]);
void  hashtbl_add(Hashtbl *htbl, int h, QuadList *elt);
Quad *list_find(QuadList *list, Quad *key[4]);
int list_length(QuadList *list);

void quad_free(Quad *);
//void free_map(Quad_map *);

/*** Constants and global elements ***/
static const int chunk_max_len = 1048576;
static const int init_size = 33550033; // size of hashtbl
static const int init_dead_size = 32;

/* The address of a leaf is a 4 digit binary number 0123
  representing the 4 bit map
  0 1
  2 3 */
extern Quad *leaves;
/*!@}*/
#endif
