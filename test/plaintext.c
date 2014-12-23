#include "plaintext.h"
#include "runlength.h"

int main(void)
{
  LifeRle_write(stdout, plaintextlex());
}
