#ifndef HASHTBL_H
#define HASHTBL_H
#include "definitions.h"
#include "bigint.h"
#include "quad.h"

/*! \defgroup hashtbl Hashtables */
/*!@{*/
typedef struct Hashtbl Hashtbl;
typedef struct QuadList QuadList;
typedef struct QuadBlock QuadBlock;
typedef struct Quad_map Quad_map;
typedef struct Map_block Map_block;

//! Hashtable of quadtrees
/*! */
struct Hashtbl
{
  int          size;      //!< Number of "buckets"
  QuadList   **tbl;       //!< Hashtable contents

  int          dead_size; //!< Current length of dead areas array
  Quad       **dead_quad; //!< Array of quadtrees representing dead areas

  int          count;     //!< Number of created and inserted elements
  QuadBlock   *blocks;    //!< Managed memory store
};

//! List of quadtrees
struct QuadList
{
  Quad      head; //!< Note that the head element is inlined as a field.
  QuadList *tail;
};

//! A faster memory allocation, malloc chunks of memory
#define BLOCK_MAX_LEN 1048576

//! Preallocated memory block for fast quadtree creation
struct QuadBlock
{
  QuadBlock *block_next;
  int        block_len;
  QuadList   block[BLOCK_MAX_LEN];
};

//! deprecated
struct Quad_map
{
  Map_block *qm_block;
  int        k;
  Quad      *v;
  Quad_map  *map_tail;
};

//! deprecated
struct Map_block
{
  int        m_block_alive;
  Map_block *next_m_block;
  int        m_block_len;
  Quad_map   m_block[BLOCK_MAX_LEN];
};

Hashtbl *hashtbl_new(rule r); //!< Create hashtbl
void hashtbl_delete(Hashtbl*); //!< Deallocate hashtbl

void hashlife_init(void); //!< Global initializer

Quad *leaf(int k); //!< `int` to leaf
Quad *dead_space(Hashtbl *htbl, int d);
  //!< Returns an area of dead cells at depth `d`

Quad *cons_quad(
  Hashtbl *htbl,
  Quad *quad[4],
  int d); //!< Hashcons operation

Quad     *map_assoc(Quad_map*, int);
Quad_map *map_add(Quad_map*, int, Quad*);

void       print_quad(Quad*);
void       hashtbl_stat(Hashtbl*);
const int *step(Hashtbl*, int[4]);
/*!@}*/
#endif
