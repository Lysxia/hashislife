#ifndef HASHLIFE_H
#define HASHLIFE_H

#include "hashtbl.h"

Quad* new_quad(Node* n, int d);

void hashlife_init(int rule[16]);

void quad_d1(Node* node, int rule[16]);

int nb_nodes();

void hash_info();

int* step(int state[4]);

#endif
