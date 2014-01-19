#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "matrix.h"
#include "parsers.h"
#include "darray.h"

void *alloc_matrix(int m, int n, size_t size)
{
  char **a = malloc(m * sizeof(char *));

  if ( !a )
    return NULL;

  int i;
  for ( i = 0 ; i < m ; i++ )
  {
    a[i] = malloc(n * size);
    if ( !a[i] )
    {
      for ( i-- ; i >= 0 ; i-- )
        free(a[i]);
      free(a);
      return NULL;
    }
  }

  return a;
}

Matrix *read_matrix(FILE *file)
{
  if (file == NULL)
	  return NULL;

  const int buffsize = 80; // 80 char + \n + \0
  char *buff = malloc((buffsize + 2) * sizeof(char));

  if ( buff == NULL )
  {
    perror("read_plain()");
    exit(1);
  }

  do
  {
    int err = fgets(buff, buffsize+2, file) == NULL;

    if ( err || strchr(buff,'\n') == NULL )
    {
      perror("read_plain()");
      free(buff);
      return NULL;
    }
  }
  while ( buff[0] == '!' );

  Darray *da = da_new(sizeof(char *));
  Matrix *matrix = malloc(sizeof(Matrix));

  if ( matrix == NULL )
  {
    perror("read_plain()");
    exit(1);
  }

  const int width = matrix->n = strchr(buff, '\n') - buff;

  do
  {
    char *endl = strchr(buff, '\n');

    if ( endl == NULL || endl - buff < width )
    {
      if ( endl == NULL )
        fprintf(stderr, "read_plain(): Unexpected EOF. Last line must be empty.\n");
      else
        fprintf(stderr, "read_plain(): Lines must be of the same length.\n");
      fflush(stderr);
      matrix->matrix = da_unpack(da, &matrix->m);
      free_matrix(matrix);
      return NULL;
    }

    da_push(da, (char *) &buff);

    buff = malloc((width + 2) * sizeof(char));

    if ( buff == NULL )
    {
      perror("read_plain()");
      exit(1);
    }
  }
  while ( fgets(buff, width + 2, file) != NULL );

  matrix->matrix = da_unpack(da, &matrix->m);

  return matrix;
}

void write_matrix(FILE *file, Matrix *matrix)
{
  int i,j;
  for ( i = 0 ; i < matrix->m ; i++ )
  {
	  for ( j = 0 ; j < matrix->n ; j++ )
      fputc(matrix->matrix[i][j], file);
    fputc('\n', file);
  }
  fflush(file);
}

void free_matrix_contents(char **matrix, int linenum)
{
  int i;

  for ( i = 0 ; i < linenum ; i++ )
    free(matrix[i]);

  free(matrix);
}

void free_matrix(Matrix *matrix)
{
  free_matrix_contents(matrix->matrix, matrix->m);
  free(matrix);
}

