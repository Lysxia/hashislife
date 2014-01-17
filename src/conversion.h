#ifndef CONVERSION_H
#define CONVERSION_H

#include "bigint.h"
#include "matrix.h"
#include "hashtbl.h"

// Assume the remaining cells are dead cells 
// A more general setting could be imagined... Toric matrix...
Quad *matrix_to_quad(Hashtbl *htbl, const Matrix *matrix);

// Draw the matrix described by q at the specified location
Matrix *quad_to_matrix(BigInt *mmin, BigInt *nmin,
                       int mlen, int nlen,
                       Quad *q);

#endif
