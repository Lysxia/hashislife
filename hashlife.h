#ifndef HASHLIFE_H
#define HASHLIFE_H

#include "hashtbl.h"

// To be called before any other function
void hashlife_init(int rule[16]);

Quad* cons_quad(Quad *quad[4], int d);

Quad* fate(Quad* quad);

Quad* dead_space(int d);

int nb_nodes();

void hash_info();

const int* step(int state[4]);

Quad *leaves;

#endif
