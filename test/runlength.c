#include <stdio.h>
#include <stdlib.h>
#include "bitmaps.h"
#include "runlength.h"

int main(void) {
  puts("RLE input:");
  struct LifeRle rle;
  if ( 0 != life_rle_read(&rle, stdin) )
  {
    puts("Incorrect file format.");
    exit(0);
  }
  puts("RLE contents");
  life_rle_write(stdout, rle);
  puts("Convert to bitmap and back again");
  BitMap bm;
  bm_new_rle(&bm, rle);
  bm_write(stdout, &bm);
  return 0;
}
