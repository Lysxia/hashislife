#ifndef MATRIX_H
#define MATRIX_H

typedef struct Matrix
{
  char **matrix;
  int m, n;
} Matrix;

#define ALIVE 'O'
#define DEAD '.'

// void **
void *alloc_matrix(int m, int n, size_t size);

Matrix *read_matrix(FILE *file);

void write_matrix(FILE *file, Matrix *matrix);

void free_matrix_contents(char **matrix, int linenum);
void free_matrix(Matrix *matrix);

#endif
