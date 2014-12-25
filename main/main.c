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

void test_quad(Hashtbl*, Quad*, BigInt *, int);

const char *get_filename_ext(const char *filename);

int main(int argc, char *argv[])
{
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

      Hashtbl *htbl = hashtbl_new(CONWAY);
      Quad *q;

      file = fopen(filename, "r");

      if ( strcmp(get_filename_ext(filename), "rle") == 0 )
      {
        struct LifeRle rle;
        switch ( life_rle_read(&rle, file) )
        {
          case 2:
            fputs("Syntax error.", stderr);
            exit(2);
          case 1:
            perror("Reading file");
            exit(1);
          case 0:
          default:
            break;
        }
        struct RleMap rle_m = align_tokens(rle.tokens);
        free(rle.tokens);
        q = rle_to_quad(htbl, rle_m);
        RleMap_delete(rle_m);
      }
      else
      {
        return 0;
      }

      fclose(file);
      test_quad(htbl, q, t, h);

      hashtbl_stat(htbl);

      bi_free(t);
      hashtbl_delete(htbl);
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

void test_quad(Hashtbl *htbl, Quad *q, BigInt *t, int h)
{
  const int m = 32, n = 80;
  //print_quad(q);
  
#if 0
  BigInt *bi_z = bi_zero();

  UMatrix um = quad_to_prgrph(bi_z, bi_z, m, n, 0, q);
#else
#define CHUNKS_LEN 1024
  struct Task task = {
    .id = 0,
    .steps = t,
    .chunks = chunks_new(sizeof(QuadTaskList), CHUNKS_LEN) };

  int d = q->depth;
  q = destiny(htbl, q, &task);
  //q = skip(htbl,q,NULL);
  //q = expand(htbl, q, q->depth+1);
  while (q->depth > d)
    q = center(htbl, q->node.n.sub, q->depth-1);

  UMatrix um = quad_to_matrix(q, h, bi_zero(), m, bi_zero(), n); 
#endif

  char **next_p;
  if ( h == 0 )
  {
    next_p = um.char_;
  }
  else
  {
    next_p = bi_to_char_mat(um.bi_, m, n, h);
  }

  matrix_write(stdout, next_p, m, n);

  if ( h == 0 )
  {
    matrix_delete((void **) um.char_);
  }
  else
  {
    // Does not free the big ints
    matrix_delete((void **) um.bi_);
    matrix_delete((void **) next_p);
  }
}
