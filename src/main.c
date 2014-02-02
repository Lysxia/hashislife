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
#include "prgrph.h"

void test_quad(Hashtbl*, Quad*, BigInt *, int);

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


      Hashtbl *htbl = hashtbl_new(conway);
      Quad *q;

      if ( strcmp(get_filename_ext(filename), "rle") == 0 )
      {
        Rle *rle = read_rle(file);

        write_rle(stdout, rle);
        
        printf("___\n");
        fflush(stdout);

        q = rle_to_quad(htbl, rle);

        printf("...\n");
        fflush(stdout);

        free_rle(rle);
      }
      else
      {
        Prgrph p = read_prgrph(file);

        q = prgrph_to_quad(htbl, p);

        //write_prgrph(stdout, p);
        free_prgrph(p);
      }

      fclose(file);
      test_quad(htbl, q, t, h);

      hashtbl_stat(htbl);

      bi_free(t);
      free_hashtbl(htbl);

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

void test_quad(Hashtbl *htbl, Quad *q, BigInt *t, int h)
{
  const int m = 32, n = 80;
  //print_quad(q);
  
#if 0
  BigInt *bi_z = bi_zero();

  UMatrix um = quad_to_prgrph(bi_z, bi_z, m, n, 0, q);
#else
  int shift_e;

  q = destiny(htbl, q, t, &shift_e);

  BigInt *bi_l = bi_power_2(shift_e - h);

  UMatrix um = quad_to_matrix(bi_l, bi_l, m, n, h, q); 
  bi_free(bi_l);
#endif

  Prgrph next_p;
  if ( !h )
  {
    next_p.prgrph = um.um_char;
  }
  else
  {
    next_p = bi_mat_to_prgrph(um.um_bi, m, n, h);
  }

  next_p.m = m;

  write_prgrph(stdout, next_p);

  //printf("Cell count (log2):%d\n", bi_log2(cell_count(q)));
  //bi_print(cell_count(q));

  //htbl_stat(htbl);

  if ( !h )
  {
    free_um_char(um, m);
  }
  else
  {
    free_um_bi(um, m);
    free_prgrph(next_p);
  }
}
