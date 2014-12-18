#ifndef HASHTBL_H
#define HASHTBL_H
#include "definitions.h"
#include "bigint.h"
#include "quad.h"

typedef struct Hashtbl Hashtbl;

Hashtbl *hashtbl_new(rule r);
void free_hashtbl(Hashtbl*);

void hashlife_init(void);

Quad *leaf(int k);
Quad *dead_space(Hashtbl *htbl, int d);
Quad *cons_quad(
  Hashtbl *htbl,
  Quad *quad[4],
  int d);

Quad     *map_assoc(Quad_map*, int);
Quad_map *map_add(Quad_map*, int, Quad*);

void       print_quad(Quad*);
void       hashtbl_stat(Hashtbl*);
const int *step(Hashtbl*, int[4]);

#endif
