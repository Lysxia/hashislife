#ifndef CONVERSION_H
#define CONVERSION_H

#include "bigint.h"
#include "matrix.h"
#include "hashtbl.h"

typedef union UMatrix
{
  char **um_char;
  const BigInt ***um_bi;
} UMatrix;

// Assume the remaining cells are dead cells 
// A more general setting could be imagined... Toric matrix...
Quad *matrix_to_quad(Hashtbl *htbl, const Matrix *matrix);

// Draw the matrix described by q at the specified location
UMatrix quad_to_matrix(BigInt *mmin, BigInt *nmin,
                          int mlen, int nlen,
                          int height, Quad *q);

Matrix *bi_mat_to_matrix(const BigInt ***bm, int m, int n, int height);

#endif
