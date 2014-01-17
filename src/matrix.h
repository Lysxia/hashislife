#ifndef MATRIX_H
#define MATRIX_H

typedef struct Matrix
{
  char **matrix;
  int m, n;
} Matrix;

#define ALIVE 'o'
#define DEAD '.'

Matrix *read_matrix(FILE *file);

void write_matrix(FILE *file, Matrix *matrix);

void free_matrix(Matrix *matrix);

#endif
