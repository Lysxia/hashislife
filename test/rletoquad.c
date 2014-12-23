#include <stdio.h>
#include <stdlib.h>

#include "bitmaps.h"
#include "conversion.h"
#include "conversion_aux.h"
#include "hashtbl.h"
#include "runlength.h"

int main(int argc, char *argv[]) {
  if ( argc > 1 ) {
    FILE *file = fopen(argv[1], "r");
    struct LifeRle rle = LifeRle_read(file);
    puts("RLE file contents");
    LifeRle_write(stdout, rle);
    struct RleMap rle_m = align_tokens(rle.tokens);
    Hashtbl *h = hashtbl_new(CONWAY);
    int m = 27, n = 33;
    matrix_write(stdout, quad_to_matrix(rle_to_quad(h, rle_m), 0, bi_zero_const, m, bi_zero_const, n).char_, m, n);
  }
  return 0;
}
