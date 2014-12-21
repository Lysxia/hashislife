#include <stdio.h>
#include <stdlib.h>
#include "bitmaps.h"
#include "runlength.h"

int main(int argc, char *argv[]) {
  if ( argc > 1 ) {
    FILE *file = fopen(argv[1], "r");
    struct RLE rle = read_rle(file);
    puts("RLE");
    fflush(stdout);
    write_rle(stdout, rle);
    fflush(stdout);
    puts("BM");
    bm_write(stdout, rle_to_bm(rle));
  }
  return 0;
}
