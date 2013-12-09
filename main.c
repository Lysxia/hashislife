#include <stdio.h>
#include <stdlib.h>
#include "bigint.h"
#include "hashtbl.h"
#include "hashlife.h"
#include "hbitmaps.h"
#include "read_gol.h"

void test_map(int**, int, int, int);

int main(int argc, char **argv)
{
  //const rule conway = 6148;
  rule r;
  FILE *file;
  int m, n, **map;
  int i,j;

  switch (argc)
  {
    case 1:
      bi_test();
      break;
    case 2:
      file = fopen(argv[1], "r");
      map = read_gol(&m, &n, &r, file);

      test_map(map, m, n, r);
      break;
  }

  return 0;
}

void test_map(int** map, int m, int n, int r)
{
  print_map(map, m, n, stdout);

  Hashtbl *htbl = hashtbl_new(r);

  Quad *q = map_to_quad(htbl, map, m, n);

  //print_quad(q);

  int i, j;

  for (i = 0 ; i < m ; i++)
    for (j = 0 ; j < m ; j++)
      map[i][j] = 0;

  quad_to_map(map, 0, 0, m, n, bi_zero, bi_zero, q);

  print_map(map, m, n, stdout);

  free(htbl);
}
