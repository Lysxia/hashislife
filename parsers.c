#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsers.h"

int parse_digit_string(char *buff, int *i)
{
  int r = 0;

  while ('0' <= buff[*i] && buff[*i] <= '8')
  {
    r |= 1 << (buff[(*i)++] - '0');
  }

  return r;
}

int parse_rule(char *buff)
{
  // s rule
  if (buff[0] != 's')
    return -1;

  int i = 1;

  int s = parse_digit_string(buff, &i);

  if (s >> 9)
    return -1;

  // b rule
  if (buff[i] != '/' || buff[i+1] != 'b')
    return -1;

  i += 2;

  int b = parse_digit_string(buff, &i);

  if (b >> 9)
    return -1;

  return (s << 9) | b;
}

int **read_gol(int *m, int *n, rule *r, FILE *file)
{
  int **life, i, j;

  if (file == NULL)
	  return NULL;

  const int buffsize = 20;
  char buff[buffsize];

  // matrix size on two lines
  int a, b;

  fgets(buff, buffsize, file);
  a = *m = atoi(buff);
 
  fgets(buff, buffsize, file);
  b = *n = atoi(buff);

  // rules
  fgets(buff, buffsize, file);

  *r = parse_rule(buff);

  if (*r == -1)
  {
    printf("in read_gol(...): Expected rule 'B*/S*'\n");
    return NULL;
  }

  //matrix
  char buff2[b+2];

  if ((life = alloc_matrix(a, b)) == NULL)
  {
	  printf("in read_gol(...): Not enough memory\n");
    return NULL;
  }

  for (i = 0 ; i < a ; i++)
  {
    fgets(buff2, b + 2, file);

	  for (j = 0 ; j < b ; j++)
	  {
	    switch (buff2[j])
	    {
		    case '.':
		      life[i][j] = 0;
		      break;
		    case 'o':
		      life[i][j] = 1;
          break;
		    default:
		      printf("in read_gol(...): Bad matrix, unrecognized character, \
                  ascii %d\n", buff2[j]);
          free_matrix(life, a);
		      return NULL;
	    }
	  }
  }

  return life;
}

void print_matrix(int **matrix, int m, int n, FILE *file)
{
  int i,j;
  for (i = 0 ; i < m ; i++)
  {
	  for (j = 0 ; j < n ; j++)
      fputc(matrix[i][j] ? 'o' : '.', file);
    fputc('\n', file);
  }
  fflush(file);
}

int **alloc_matrix(int m, int n)
{
  int **matrix, i;

  if (NULL == (matrix = malloc(m * sizeof(int*))))
	  return NULL;

  for (i = 0 ; i < m ; i++)
	  if (NULL == (matrix[i] = malloc(n * sizeof(int))))
	  {
      free_matrix(matrix, i);
	    return NULL;
	  }

  return matrix;
}

void free_matrix(int **matrix, int m)
{
  int i;

  for (i = 0 ; i < m ; i++)
    free(matrix[i]);

  free(matrix);
}

#ifdef DEBUG

void test_rle_token(char *filename)
{
  FILE *file;
  if ((file = fopen(filename, "r")) == NULL)
    return;

  char tag;
  int len = rle_token(file, &tag);

  while (len)
  {
    if (len == -1)
    {
      printf("///\n");
      return;
    }
    printf("%d%c\n", len, tag);
    len = rle_token(NULL, &tag);
  }

  printf("!\n");
}

#endif
