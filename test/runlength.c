#include <stdio.h>
#include <stdlib.h>
#include "bitmaps.h"
#include "runlength.h"

int main(int argc, char *argv[]) {
  if ( argc > 1 ) {
    FILE *file = fopen(argv[1], "r");
    struct LifeRle rle = LifeRle_read(file);
    puts("RLE file contents");
    LifeRle_write(stdout, rle);
    puts("Convert to bitmap and back again");
    bm_write(stdout, bm_new_rle(rle));
  }
  return 0;
}
