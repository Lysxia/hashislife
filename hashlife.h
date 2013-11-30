#ifndef HASHLIFE_H
#define HASHLIFE_H

#include "hashtbl.h"

Quad* map_to_quad(int** map, int m, int n);

Quad* cons_quad(Quad *quad[4], int d);

void hashlife_init(int rule[16]);

void quad_d1(Quad* quad[4], int rule[16]);

int nb_nodes();

void hash_info();

const int* step(int state[4]);

#endif
