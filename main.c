#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "darray.h"
#include "bigint.h"
#include "hashtbl.h"
#include "hashlife.h"
#include "conversion.h"
#include "parsers.h"
#include "runlength.h"
#include "matrix.h"

void test_matrix(Matrix *, rule);

const char *get_filename_ext(const char *filename);

int main(int argc, char *argv[])
{
  const rule conway = 6148;
  const char *filename = argv[1];
  FILE *file;

  switch ( argc )
  {
    case 1:
      bi_test();
      break;
    case 2:
      file = fopen(filename, "r");

      if ( strcmp(get_filename_ext(filename), "rle") == 0 )
      {
        Rle *rle = read_rle(file);

        write_rle(stdout, rle);

        free_rle(rle);
      }
      else
      {
        Matrix *mat = read_matrix(file);

        test_matrix(mat, conway);

        free_matrix(mat);
      }

      break;
  }

  return 0;
}

const char *get_filename_ext(const char *filename)
{
  const char *dot = strrchr(filename, '.');
  if ( !dot || dot == filename) return "";
  return dot + 1;
}

void test_matrix(Matrix* mat, rule r)
{
  printf("%d\n", r);
  write_matrix(stdout, mat);

  Hashtbl *htbl = hashtbl_new(r);

  Quad *q = matrix_to_quad(htbl, mat);

  BigInt *bi_z = bi_zero();

  //print_quad(q);
  
  Matrix *new_mat = quad_to_matrix(bi_z, bi_z, mat->m, mat->n, q);

  printf("HEYY\n");
  write_matrix(stdout, new_mat);
  exit(2);
  
  /*
  int shift_e;
  BigInt one = bi_from_int(910);

  q = destiny(htbl, q, one, &shift_e);

  printf("Destiny.\n");

  int l = 1 << shift_e ;

  const int side_m = 50;
  const int side_n = 128;
  int **mat2 = alloc_matrix(side_m, side_n);

  BigInt bi_l = bi_from_int(l);

  quad_to_matrix(mat2, 0, 0, side_m, side_n, bi_l, bi_l, q);

  print_matrix(mat2, side_m, side_n, stdout);

  bi_free(one);
  bi_free(bi_l);
  free_matrix(mat2, side_m);
  free(htbl);
  */
}
