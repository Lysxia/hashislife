#ifndef HASHLIFE_H
#define HASHLIFE_H

#include "hashtbl.h"

Quad* hashlife(int** map, int m, int n,
                int mmin, int mmax, int nmin, int nmax, int d);

Quad* _hashlife(int** map, int m, int n,
                int mmin, int mmax, int nmin, int nmax, int d);

Quad* mk_quad(Quad *quad[4], int d);

void hashlife_init(int rule[16]);

void quad_d1(Quad* quad[4], int rule[16]);

int nb_nodes();

void hash_info();

int* step(int state[4]);

#endif
