#ifndef HBITMAPS_H
#define HBITMAPS_H

// Assume the remaining cells are dead cells 
// A more general setting could be imagined... Toric matrix...
Quad *matrix_to_quad(Hashtbl *htbl, int **matrix, int mlen, int nlen);

// Draw the matrix described by q at the specified location
void quad_to_matrix(int **matrix, int m, int n,
                 int mlen, int nlen,
                 BigInt mmin, BigInt nmin, Quad *q);

#endif
