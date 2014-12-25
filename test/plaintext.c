#include "plaintext.h"
#include "runlength.h"

int main(void)
{
  struct LifeRle rle;
  if ( plaintextlex(&rle) == 0 )
    life_rle_write(stdout, rle);
  else
    puts("Incorrect format.");
  return 0;
}
