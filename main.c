#include <stdio.h>
#include <stdlib.h>
#include "hashtbl.h"
#include "hashlife.h"
#include "hbitmaps.h"
#include "read_gol.h"

void test();

int main(int argc, char **argv)
{
  rule r = 6148;
  FILE *file;
  int m, n, **map;
  int i,j;

  hashlife_init(r);

  switch (argc)
  {
    case 1:
      hash_info();
      test();
      break;
    case 2:
      file = fopen(argv[1], "r");
      map = read_gol(&m, &n, &r, file);
      fclose(file);
      print_map(map, m, n, stdout);
      fputc('\n', stdout);

      Quad *q = map_to_quad(map, m, n);

      // print_quad(q);

      for (i = 0 ; i < m ; i++)
        for (j = 0 ; j < m ; j++)
          map[i][j] = 0;

      quad_to_map(map, 0, 0, 0, 0, m, n, q);
      print_map(map, m, n, stdout);
      break;
  }

  return 0;
}

void test()
{
  int state[4], i;
  while (1)
  {
    for (i=0 ; i<4 ; i++)
    {
      scanf(" %d", state+i);
      if (state[i] < 0)
        return;
    }
    const int *map = step(state);
    for (i=0 ; i<4 ; i++)
      printf("%d ",map[i]);
    printf("\n");
  }
}
