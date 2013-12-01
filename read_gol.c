#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "read_gol.h"

int** read_gol(int *m, int *n, rule *r, FILE *file)
{
  int **life, i, j;

  if (file == NULL)
	  return NULL;

  const int buffsize = 20;
  char buff[buffsize];

  // map size on two lines
  int a, b;

  fgets(buff, buffsize, file);
  a = *m = atoi(buff);
 
  fgets(buff, buffsize, file);
  b = *n = atoi(buff);

  // rules
  fgets(buff, buffsize, file);

  *r = 0;
  i = 0;

  // B rule
  if (buff[i++] != 'B')
  {
    printf("in read_gol(): Expected rule 'B*/S*'\n");
    return NULL;
  }

  while (buff[i] >= '0' && buff[i] <= '8')
    *r |= 1 << (buff[i++] - '0');

  //S rule
  if (buff[i] != '/' || buff[i+1] != 'S')
  {
    printf("in read_gol(): Expected rule 'B*/S*'\n");
    return NULL;
  }

  i += 2;

  while (buff[i] >= '0' && buff[i] <= '8')
    *r |= 1 << (buff[i++] - '0' + 9);

  //map
  char buff2[b+2];

  if (NULL == (life = malloc(a * sizeof(int*))))
  {
	  printf("in read_gol(): Not enough memory\n");
	  return NULL;
  }

  for (i = 0 ; i < a ; i++)
  {
	  if (NULL == (life[i] = malloc(b * sizeof(int))))
	  {
	    printf("in read_gol(): Not enough memory\n");
	    while (i-- > 0)
		    free(life[i]);
	    return NULL;
	  }

    fgets(buff2, b + 2, file);

	  for (j = 0 ; j < b ; j++)
	  {
	    switch (buff2[j])
	    {
		    case '0':
		      life[i][j] = 0;
		      break;
		    case '1':
		      life[i][j] = 1;
          break;
		    default:
		      printf("in read_gol(): Bad map, unrecognized character, ascii %d\n", buff2[j]);
		      i++;
		      while (i-->0)
			      free(life[i]);
		      return NULL;
	    }
	  }
  }

  return life;
}

void print_map(int **map, int m, int n, FILE *file)
{
  int i,j;
  for (i = 0 ; i < m ; i++)
  {
	  for (j = 0 ; j < n ; j++)
      fputc(map[i][j] ? '1' : '0', file);
    fputc('\n', file);
  }
}
