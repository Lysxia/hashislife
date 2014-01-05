#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bitmaps.h"
#include "parsers.h"
#include "plainparser.h"

BitMap *read_plain(FILE *file)
{
  if (file == NULL)
	  return NULL;

  Darray da;
  
  da_init(&da, sizeof(char *));

  const int buffsize = 80; // 80 char + \n + \0
  char *buff = malloc((buffsize+2) * sizeof(char));

  BitMap *bm = bm_new(RAW);

  if ( buff == NULL || bm == NULL )
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
      bm_free(bm);
      free(buff);
      return NULL;
    }

    rule s = parse_rule(buff + 1);
    if ( s != (rule) -1 && buff[0] == '!' )
      bm->r = s;
  }
  while ( buff[0] == '!' );

  const int width = bm->x = strchr(buff, '\0') - buff - 1;

  do
  {
    da_append(&da, (char *) &buff);
    
    buff = malloc((width + 2) * sizeof(char));

    if ( buff == NULL )
    {
      perror("read_plain()");
      exit(1);
    }
  }
  while ( fgets(buff, width + 2, file) != NULL );

  bm->y = da.array_length;

  bm->map.raw = (char **) da.da;

  return bm;
}

void write_plain(FILE *file, BitMap *bm)
{
  write_matrix(file, bm->map.raw, bm->y, bm->x);
}

void write_matrix(FILE *file, char **matrix, int m, int n)
{
  int i,j;
  for ( i = 0 ; i < m ; i++ )
  {
	  for ( j = 0 ; j < n ; j++ )
      fputc(matrix[i][j], file);
    fputc('\n', file);
  }
  fflush(file);
}

int **alloc_matrix(int m, int n)
{
  int **matrix, i;

  if ( NULL == (matrix = malloc(m * sizeof(int*))) )
	  return NULL;

  for ( i = 0 ; i < m ; i++ )
	  if ( NULL == (matrix[i] = malloc(n * sizeof(int))) )
	  {
      free_matrix(matrix, i);
	    return NULL;
	  }

  return matrix;
}

void free_matrix(char **matrix, int m)
{
  int i;

  for ( i = 0 ; i < m ; i++ )
    free(matrix[i]);

  free(matrix);
}

