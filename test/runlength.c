#include <stdio.h>
#include <stdlib.h>
#include "bitmaps.h"
#include "runlength.h"

int main(void) {
  puts("RLE input:");
  struct LifeRle rle = life_rle_read(stdin);
  puts("RLE contents");
  life_rle_write(stdout, rle);
  puts("Convert to bitmap and back again");
  bm_write(stdout, bm_new_rle(rle));
  return 0;
}
