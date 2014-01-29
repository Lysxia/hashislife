#ifndef HASHTBL_H
#define HASHTBL_H
#include <stdint.h>
#include "definitions.h"
#include "bigint.h"

typedef struct Hashtbl Hashtbl;
typedef struct Quad_map Quad_map;

typedef struct Quad Quad;

union Node
{
  // internal node
  struct
  {
    Quad     *sub[4]; // subtrees : 0:upper left,  1:upper right,
    Quad_map *next;   //            2:bottom left, 3:bottom right
  } n;

  // leaf
  struct
  {
    int map[4];
    /* 0 1
     * 2 3
     * */
  } l;
};

struct Quad
{
  int         depth;      // quad tree for a square map with side 2^(depth+1)
  BigInt     *cell_count;
  union Node  node;
};

/********************/

Hashtbl *hashtbl_new(rule r);
void free_hashtbl(Hashtbl*);

void hashlife_init(void);

Quad *leaf(int k);
Quad *dead_space(Hashtbl *htbl, int d);
Quad *cons_quad(
  Hashtbl *htbl,
  Quad *quad[4],
  int d);

Quad *map_assoc(Quad_map*, int);
Quad_map *map_add(Quad_map*, int, Quad*);

void print_quad(Quad*);
void hashtbl_stat(Hashtbl*);
const int *step(Hashtbl*, int[4]);

#endif
