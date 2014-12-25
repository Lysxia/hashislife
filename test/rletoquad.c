#include <stdio.h>
#include <stdlib.h>

#include "bitmaps.h"
#include "conversion.h"
#include "conversion_aux.h"
#include "hashtbl.h"
#include "runlength.h"

int main(void) {
  struct LifeRle rle;
  if ( 0 != life_rle_read(&rle, stdin) )
  {
    puts("Incorrect file format.");
    exit(0);
  }
  puts("RLE file contents");
  life_rle_write(stdout, rle);
  struct RleMap rle_m;
  align_tokens(&rle_m, rle.tokens); // Should succeed
  Hashtbl *h = hashtbl_new(CONWAY);
  int m = 27, n = 33;
  matrix_write(stdout, quad_to_matrix(rle_to_quad(h, rle_m), 0, bi_zero_const, m, bi_zero_const, n).char_, m, n);
  return 0;
}
