#include <string.h>

#include "biblock.h"

void bi_block_copy(
  BiBlock *a_digits, const size_t a_length,
  const BiBlock *b_digits, const size_t b_length)
{
  const int b_sign = SIGN(b_digits[b_length-1]) ? -1 : 0;
  memcpy(a_digits, b_digits, b_length * sizeof(*a_digits));
  memset(a_digits + b_length, b_sign,
    (a_length - b_length) * sizeof(*a_digits));
}

void bi_block_add_to(
  BiBlock *a_digits, const size_t a_length, // max(b_length, c_length)
  const BiBlock *c_digits, const size_t c_length)
{
  const BiBlock c_sign = -SIGN(c_digits[c_length-1]);
  BiBlock carry = 0;
  // add b to a with overflow check
#define ADD_TO_OF_CHECK(a,b) ( (a+=b) < b )
  for ( size_t i = 0 ; i < a_length ; i++ )
  {
    carry = ADD_TO_OF_CHECK(a_digits[i], carry);
    const BiBlock c_ = ( i < c_length ) ? c_digits[i] : c_sign;
    carry |= ADD_TO_OF_CHECK(a_digits[i], c_); // at most one overflow
  }
#undef ADD_TO_OF_CHECK
}

void bi_block_sub_from(
  BiBlock *a_digits, const size_t a_length,
  const BiBlock *c_digits, const size_t c_length)
{
  const BiBlock c_sign = -SIGN(c_digits[c_length-1]);
  BiBlock carry = 0;
  // subtract b from a with overflow check
#define SUB_FROM_OF_CHECK(a,b) ( a < b ); a -= b
  for ( size_t i = 0 ; i < a_length ; i++ )
  {
    carry = SUB_FROM_OF_CHECK(a_digits[i], carry);
    const BiBlock c_ = ( i < c_length ) ? c_digits[i] : c_sign;
    carry |= SUB_FROM_OF_CHECK(a_digits[i], c_);
  }
#undef ADD_TO_OF_CHECK
}


void bi_block_set(BiBlock *a, const size_t k, const int bit)
{
  size_t pos = k / BiBlock_bit, ofs = k % BiBlock_bit;
  a[pos] |= (BiBlock) 1 << ofs;
  a[pos] ^= (BiBlock) !bit << ofs;
}
