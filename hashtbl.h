#ifndef HASHTBL_H
#define HASHTBL_H
#include <stdint.h>

typedef int rule;
typedef struct Hashtbl Hashtbl;
typedef struct QuadMap QuadMap;

typedef struct Quad Quad;

union Node
{
  // internal node
  struct
  {
    Quad *sub[4]; // subtrees : 0-upperleft, 1-upperright, 2-bottomleft, 3-bottomright
    QuadMap *next;
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
  int depth; // quad tree for a square map with side 2^(depth+1)
  union Node node;
};

/********************/

Hashtbl *hashtbl_new(rule r);
void hashtbl_free(Hashtbl*);

Quad *leaf(int k);
Quad *dead_space(Hashtbl *hashtbl, int d);
Quad *cons_quad(Hashtbl *htbl, Quad *quad[4], int d);

Quad *map_assoc(QuadMap*, int);
QuadMap *map_add(QuadMap**, int, Quad*);

void print_quad(Quad*);
void htbl_stat(Hashtbl*);
int nb_nodes(Hashtbl*);
const int *step(Hashtbl*, int[4]);

#endif
