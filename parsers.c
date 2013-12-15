#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "darray.h"
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

#define RLE_LINE_LENGTH 100

int rle_token(FILE *new_file, char *tag)
{
  static FILE *file = NULL;
  static char buff[RLE_LINE_LENGTH];
  static int i;

  if (new_file != NULL)
  {
    file = new_file;
    buff[0] = '\0';
    i = 0;
  }
  else if (file == NULL)
    return -1;

  i += strspn(buff + i, " \n\t");

  if (buff[i] == '\0')
  {
    i = 0;
    if (fgets(buff, RLE_LINE_LENGTH, file) == NULL)
      return -1;
  }
  else if (buff[i] == '!')
    return 0;

  int len;

  len = atoi(buff + i);
  i += strspn(buff +i, "0123456789");
  *tag = buff[i++];

  return len ? len : 1;
}

// bm: Bit Map
// rle: Run Length Encoding

BitMap *bm_new(enum MapType t);
void rle_map_free(Darray *rle);
int read_rle_(Darray *rle, FILE *file);
Rle_line *bm_rle_newline(Darray *rle, int line_num);

void bm_free(BitMap *map)
{
  switch (map->map_type)
  {
    case RLE:
      rle_map_free(&map->map.rle);
      break;
  }
}

BitMap *read_rle(FILE *file)
{
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
  BitMap *map = bm_new(RLE);

  switch (sscanf(buff, " x = %d , y = %d , r = %21s ", &map->x, &map->y, s))
  {
    case 3:
      map->r = parse_rule(s);
      if (map->r != -1)
        break;
    case EOF:
    case 1:
      bm_free(map);
      printf("in read_rle(...): Bad format\n");
      return NULL;
  }

  if (read_rle_(&map->map.rle, file))
    return map;
  else
  {
    free(map);
    return NULL;
  }
}

int read_rle_(Darray *rle, FILE *file)
{
  int c = 0, i = 0, j = 0;

  Rle_line cur_line;

  da_init(&cur_line.line_rle, sizeof(int));

  int run_len;
  char tag;

  run_len = rle_token(file, &tag);

  while (run_len)
  {
    if (run_len < 0)
    {
      printf("in read_rle(...): Invalid syntax\n");
      rle_map_free(rle);
      return 0;
    }

    switch (tag)
    {
      case '$':
        if (c > 0)
        {
          if (c % 2 == 1)
            da_append(&cur_line.line_rle, (char *) &j);
          cur_line.line_num = i;
          da_append(rle, (char *) &cur_line);

          c = 0;
          da_init(&cur_line.line_rle, sizeof(int));
        }
        i += run_len;
        j = 0;
        break;
      case 'o':
        if (c % 2 == 0)
        {
          da_append(&cur_line.line_rle, (char *) &j);
          c++;
        }
        j += run_len;
        break;
      case 'b':
        if (c % 2 == 1)
        {
          da_append(&cur_line.line_rle, (char *) &j);
          c++;
        }
        j += run_len;
        break;
      default:
        printf("in read_rle(): Unrecognized syntax\n");
        rle_map_free(rle);
        return 0;
    }

    run_len = rle_token(NULL, &tag);
  }

  if (c > 0)
  {
    if (c % 2 == 1)
    {
      if (c % 2 == 1)
        da_append(&cur_line.line_rle, (char *) &j);
      cur_line.line_num = i;
      da_append(rle, (char *) &cur_line);
    }
  }

  return 1;
}

int itoa(char *dest, int src)
{
  int i = 0, j;
  do
  {
    dest[i++] = '0' + src % 10;
    src /= 10;
  } while (src > 0);

  for (j = 0 ; j < i / 2 ; j++)
  {
    char tmp = dest[i-j-1];
    dest[i-j-1] = dest[j];
    dest[j] = tmp;
  }

  dest[i] = '\0';

  return i;
}

void bounded_lines(int run_count, char tag, FILE *new_file)
{
  static FILE *file = NULL;
  static char a[20];
  static int line;

  if (new_file != NULL)
  {
    file = new_file;
    line = 0;
  }
  else if (file == NULL)
    return;

  int written = itoa(a, run_count);

  if (run_count == 1)
    a[0] = tag;
  else if (run_count > 1)
  {
    a[written] = tag;
    a[written+1] = '\0';
    written++;
  }
  else
    return;

  if ((line += written) > 70)
  {
    printf("\n");
    line = 0;
  }
  
  printf("%s", a);
}

void print_rle(BitMap *map, FILE *file)
{
  int l, c, line = 0;

  bounded_lines(0, '\0', file);

  for (l = 0 ; l < map->len ; l++)
  {
    if (l > 0)
      bounded_lines(map->line_num[l] - map->line_num[l-1], '$', NULL);

    for (c = 1 ; c < map->line_len[l] ; c++)
    {
      if (c == 0)
        bounded_lines(map->map[l][0], 'b', NULL);
      else
        bounded_lines(map->map[l][c] - map->map[l][c-1],
                      c % 2 ? 'o' : 'b',
                      NULL);
    }
  }
  printf("\n!\n");
  fflush(file);
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

/**/

BitMap *bm_new(enum MapType t)
{
  BitMap *map = malloc(sizeof(BitMap));

  switch (t)
  {
    case RLE:
      da_init(&map->map.rle, sizeof(Rle_line));
      break;
  }

  map->map_type = t;

  map->corner_x = map->corner_y = 0;
  map->x = map->y = 0;
  map->r = 0;

  return map;
}

void rle_map_free(Darray *rle)
{
  int l;
  for (l = 0 ; l < rle->array_length ; l++)
    da_clear(&((BitMap_RLE_line *) rle->da)[l].line_rle);

  da_clear(rle);
}

Rle_line *bm_rle_newline(Darray *rle, int line_num)
{
  Rle_line *dest = da_append(rle, NULL);

  da_init(&dest->line_rle, sizeof(int));
  dest->line_num = line_num;

  return dest;
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
