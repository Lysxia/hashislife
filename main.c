#include <stdio.h>
#include <stdlib.h>
#include "bigint.h"
#include "hashtbl.h"
#include "hashlife.h"
#include "hbitmaps.h"
#include "read_gol.h"

void test_matrix(int**, int, int, int);

int main(int argc, char **argv)
{
  //const rule conway = 6148;
  rule r;
  FILE *file;
  int m, n, **mat;
  int i,j;

  switch (argc)
  {
    case 1:
      bi_test();
      break;
    case 2:
      file = fopen(argv[1], "r");
      mat = read_gol(&m, &n, &r, file);

      test_matrix(mat, m, n, r);
      
      free_matrix(mat, m);

      break;
  }

  return 0;
}

void test_matrix(int** mat, int m, int n, int r)
{
  print_matrix(mat, m, n, stdout);

  Hashtbl *htbl = hashtbl_new(r);

  Quad *q = matrix_to_quad(htbl, mat, m, n);

  //print_quad(q);
  
  int shift_e;
  BigInt one = bi_plus_int(bi_zero, 1);

  q = destiny(htbl, q, one, &shift_e);

  printf("Destiny.\n");

  int l = 1 << shift_e;

  int **mat2 = alloc_matrix(l, l);

  quad_to_matrix(mat2, 0, 0, l, l, bi_zero, bi_zero, q);

  print_matrix(mat, m, n, stdout);

  free(htbl);
}
