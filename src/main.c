#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "darray.h"
#include "bigint.h"
#include "hashtbl.h"
#include "hashlife.h"
#include "lifecount.h"
#include "conversion.h"
#include "parsers.h"
#include "runlength.h"
#include "matrix.h"

void test_matrix(Matrix *, rule, BigInt *, int);

const char *get_filename_ext(const char *filename);

const char *collect_arg3(char *argv, BigInt **t);

int main(int argc, char *argv[])
{
  const rule conway = 6152; // parse_rule("b3/s23");

  int h = 0;
  BigInt *t;
  char *filename;
  FILE *file;

  switch ( argc )
  {
    case 4:
      h = atoi(argv[3]);
    case 3:
      filename = argv[1];
      t = bi_from_string(argv[2], 10);

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

        test_matrix(mat, conway, t, h);

        bi_free(t);
        free_matrix(mat);
      }

      break;
    case 1:
#if 0
      bi_test();
#endif
    default:
      printf("usage: %s (filename) (t:integer) [h:integer]\n", argv[0]);
  }

  return 0;
}

const char *get_filename_ext(const char *filename)
{
  const char *dot = strrchr(filename, '.');
  if ( !dot || dot == filename) return "";
  return dot + 1;
}

const char *collect_arg3(char *argv, BigInt **t)
{
  return NULL;
}

void test_matrix(Matrix* mat, rule r, BigInt *t, int h)
{
#if 0
  write_matrix(stdout, mat);
#endif

  Hashtbl *htbl = hashtbl_new(r);

  Quad *q = matrix_to_quad(htbl, mat);

  //BigInt *bi_z = bi_zero();

  //print_quad(q);
  
#if 0
  Matrix *new_mat = quad_to_matrix(bi_z, bi_z, mat->m, mat->n, q);

  write_matrix(stdout, new_mat);
  
  free_matrix(new_mat);
#endif

  int shift_e;

  printf("Destiny...\n");

  q = destiny(htbl, q, t, &shift_e);

  const int side_m = 32;
  const int side_n = 80;

  BigInt *bi_l = bi_power_2(shift_e - h);

  UMatrix um = quad_to_matrix(bi_l, bi_l, side_m, side_n, h, q); 

  Matrix *next_mat;
  if ( !h )
  {
    next_mat = malloc(sizeof(Matrix));

    next_mat->matrix = um.um_char;
  }
  else
    next_mat = bi_mat_to_matrix(um.um_bi, side_m, side_n, h);

  next_mat->m = side_m;
  next_mat->n = side_n;

  write_matrix(stdout, next_mat);

  printf("Cell count (log2):%d\n", bi_log2(cell_count(q)));
  bi_print(cell_count(q));

  //htbl_stat(htbl);

  free_matrix(next_mat);
  bi_free(bi_l);
  free(htbl);
}
