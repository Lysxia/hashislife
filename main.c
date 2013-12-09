#include <stdio.h>
#include <stdlib.h>
#include "bigint.h"
#include "hashtbl.h"
#include "hashlife.h"
#include "conversion.h"
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
  printf("%d\n", r);
  print_matrix(mat, m, n, stdout);

  Hashtbl *htbl = hashtbl_new(r);

  Quad *q = matrix_to_quad(htbl, mat, m, n);

  //print_quad(q);
  
  int shift_e;
  BigInt one = bi_plus_int(bi_zero, 900);

  q = destiny(htbl, q, one, &shift_e);

  printf("Destiny.\n");

  int l = 1 << shift_e ;

  const int side_m = 50;
  const int side_n = 128;
  int **mat2 = alloc_matrix(side_m, side_n);

  BigInt bi_l = bi_plus_int(bi_zero,l);

  quad_to_matrix(mat2, 0, 0, side_m, side_n, bi_l, bi_l, q);

  print_matrix(mat2, side_m, side_n, stdout);

  bi_free(one);
  bi_free(bi_l);
  free_matrix(mat2, side_m);
  free(htbl);
}
