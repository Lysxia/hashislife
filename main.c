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
  const rule conway = 6152; // parse_rule("b3/s23");

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
#if 0
  write_matrix(stdout, mat);
#endif

  Hashtbl *htbl = hashtbl_new(r);

  Quad *q = matrix_to_quad(htbl, mat);

  BigInt *bi_z = bi_zero();

  //print_quad(q);
  
#if 0
  Matrix *new_mat = quad_to_matrix(bi_z, bi_z, mat->m, mat->n, q);

  write_matrix(stdout, new_mat);
  
  free_matrix(new_mat);
#endif

  int shift_e;
  BigInt *steps = bi_from_int(123913012);

  printf("Destiny...\n");

  q = destiny(htbl, q, steps, &shift_e);

  const int side_m = 30;
  const int side_n = 120;

  BigInt *bi_l = bi_power_2(shift_e);

  Matrix *next_mat = quad_to_matrix(bi_l, bi_l, side_m, side_n, q);

  write_matrix(stdout, next_mat);

  //htbl_stat(htbl);

  free_matrix(next_mat);
  bi_free(steps);
  bi_free(bi_l);
  free(htbl);
}
