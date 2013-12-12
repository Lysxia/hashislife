#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_gol.h"

int parse_digit_string(char *buff, int *i)
{
  int r = 0;

  while ('0' <= buff[*i] && buff[*i] <= '8')
    r |= 1 << (buff[(*i)++] - '0');

  return r;
}

int parse_rule(char *buff)
{
  int i = 0;

  // s rule
  if (buff[0] != 's')
    return -1;

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

BitMap *bm_new(enum MapType t)
{
  BitMap *map = malloc(sizeof(BitMap));

  map->map_type = t;

  map->x = map->y = map->r = 0;

  map->map = NULL;
  map->len = 0;
  map->line_num = NULL;
  map->line_len = NULL;
  
  return map;
}

void bm_rle_newline(BitMap *map)
{
  if (map->len == 0)
  {
    map->map = malloc(sizeof(int*));
    map->line_num = malloc(sizeof(int));
    map->line_len = malloc(sizeof(int));

    map->map[0] = malloc(sizeof(int));

    map->len++;
  }
  else if (!(map->len & (map->len - 1)))
  {
    map->map = realloc(map->map, 2 * map->len * sizeof(int*));
    map->line_num = realloc(map->map, 2 * map->len * sizeof(int));
    map->line_len = realloc(map->map, 2 * map->len * sizeof(int));

    map->map[map->len] = malloc(sizeof(int));

    map->len++;
  }
}

void append(int **list, int i, int v)
{
  if (!(i & (i - 1)))
  {
    *list = realloc(*list, 2 * i);
  }

  (*list)[i] = v;
}

#define RLE_LINE_LENGTH 100

int rle_token(FILE *file, char *tag)
{
  static char buff[RLE_LINE_LENGTH];

  printf("Not implemented\n");
  exit(2);
}

BitMap *read_rle(FILE *file)
{
  int x, y, r;
  char buff[RLE_LINE_LENGTH];

  do
  {
    if (fgets(buff, RLE_LINE_LENGTH, file) == NULL)
    {
      printf("in read_rle(...): Error on input\n");
      return NULL;
    }
  } while (buff[0] == '#');

  char s[22];

  switch (sscanf(buff, " x = %d , y = %d , r = %21s ", &x, &y, s))
  {
    case 3:
      r = parse_rule(s);
      if (r != -1)
        break;
    case EOF:
    case 1:
      printf("in read_rle(...): Bad format\n");
      return NULL;
  }

  int c = 0, i = 0, j = 0;

  BitMap *map = bm_new(RLE);
  int run_len;
  char tag;

  map->x = x;
  map->y = y;
  map->r = r;

  run_len = rle_token(file, &tag);
  
#define l map->len
  while (run_len)
  {
    if (run_len < 0)
    {
      printf("in read_rle(...): Invalid syntax\n");
      for (i = 0 ; i < l ; i++)
        free(map->map[i]);
      free(map->map);
      free(map);
      return NULL;
    }

    switch (tag)
    {
      case '$':
        if (c > 0)
        {
          if (c % 2 == 1)
          {
            append(&(map->map[l]), c, j);
            c++;
          }
          map->line_len[l] = c;
          map->line_num[l] = i;
          c = 0;
        }
        i++;
        break;
      case 'o':
        if (c % 2 == 0)
        {
          if (c == 0)
            bm_rle_newline(map);
          append(&(map->map[l]), c, j);
          c++;
        }
        j += len;
        break;
      case 'b':
        if (c % 2 == 1)
        {
          append(&(map->map[l]), c, j);
          c++;
        }
        j += len;
        break;
      default:
        printf("in read_rle(): Unrecognized syntax\n");
        for (i = 0 ; i < l ; i++)
          free(map->map[i]);
        free(map->map);
        free(map);
        return NULL;
    }

    len = rle_token(NULL, &tag);
  }
#undef l

  return map;
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
		      printf("in read_gol(...): Bad matrix, unrecognized character, ascii %d\n", buff2[j]);
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
