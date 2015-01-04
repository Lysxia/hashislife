#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bigint.h"
#include "definitions.h"

#define COMPOSITE(a) (a)->digits.composite
#define SIMPLE(a) (a)->digits.simple
#define LENGTH(a) (a)->length
#define MAX_LENGTH(a) (a)->max_length
#define IS_SIMPLE(a) 0 == (a)->max_length
#define IS_COMPOSITE(a) 0 < (a)->max_length
//! Sign bit
#define SIGN(x) ((x) >> (BiBlock_bit - 1))
//! Uniform view of the representation
#define DIGITS(a) (IS_SIMPLE(a) ? &SIMPLE(a) : COMPOSITE(a))

const BigInt bi_zero_const_ = {
  .digits = { .simple = 0 },
  .length = 1,
  .max_length = 0
};

const BigInt * const bi_zero_const = &bi_zero_const_;

void bi_simple(BigInt *a, const BiBlock n)
{
  MAX_LENGTH(a) = 0;
  LENGTH(a) = 1;
  SIMPLE(a) = n;
}

/*! This function should only be used on previously initialized structures.
  The result is an uninitialized one. */
void bi_clear(BigInt *a)
{
  if ( IS_COMPOSITE(a) )
    free(COMPOSITE(a));
}

/*! Simplifies leading 0's or 1's depending on the sign of the number
  represented by `digits`. Will copy the remaining contents of `digits`,
  allocating a new array if necessary.

  \param length must be nonzero.

  Return `0` if success, return `1` if failure (without modifying `a`). */
int bi_normalize(BigInt *a, const BiBlock *digits, size_t length)
{
  for ( ; 1 < length ; length-- )
    if ( digits[length-1] != -(digits[length-2] >> (BiBlock_bit - 1)) )
      break;
  if ( length == 1 )
  {
    MAX_LENGTH(a) = 0;
    LENGTH(a) = 1;
    SIMPLE(a) = digits[0];
  }
  else
  {
    BiBlock *digits_ = malloc(length * sizeof(*digits_));
    if ( NULL == digits_ )
      return 1;
    MAX_LENGTH(a) = length;
    LENGTH(a) = length;
    COMPOSITE(a) = memcpy(digits_, digits, length * sizeof(*digits_));
  }
  return 0;
}

int bi_is_zero(const BigInt *a)
{
  return ( IS_SIMPLE(a)
    ? 0 == SIMPLE(a)
    : (1 == LENGTH(a) && 0 == COMPOSITE(a)[0]) );
}

/*! Undefined when `a` is negative, `0` when `a` is zero */
size_t bi_log2(const BigInt *a)
{
  size_t d = BiBlock_bit, nb_prec_blocks = LENGTH(a) - 1;
  const BiBlock last_block = DIGITS(a)[LENGTH(a)-1];

  for ( size_t d = BiBlock_bit ; d > 0 && 0 == (last_block >> (d - 1)) ; d--)
    ;

  return d + nb_prec_blocks * BiBlock_bit;
}

int bi_copy(BigInt *a, const BigInt *b)
{
  if ( IS_SIMPLE(b) )
  {
    bi_simple(a, SIMPLE(b));
    return 0;
  }
  else
    return bi_normalize(a, COMPOSITE(b), LENGTH(b));
}

void bi_block_copy(
  BiBlock *a_digits, const size_t a_length,
  const BiBlock *b_digits, const size_t b_length)
{
  const int b_sign = SIGN(b_digits[b_length-1]) ? -1 : 0;
  memcpy(a_digits, b_digits, b_length * sizeof(*a_digits));
  memset(a_digits + b_length, b_sign,
    (a_length - b_length) * sizeof(*a_digits));
}

int bi_slice(const BigInt *a, const size_t k)
{
  size_t pos = k / BiBlock_bit, ofs = k % BiBlock_bit;
  const BiBlock *a_digits = DIGITS(a);
  BiBlock slice;

  if ( pos == LENGTH(a) - 1
    || BiBlock_bit - ofs > CHAR_BIT * sizeof(int) )
    slice = -SIGN(a_digits[pos]) << (BiBlock_bit - ofs) | a_digits[pos] >> ofs;
  else if ( pos < LENGTH(a) - 1 )
    slice = a_digits[pos+1] << (BiBlock_bit - ofs) | a_digits[pos] >> ofs;
  else
    slice = 0;
  return slice & INT_MAX;
}

int bi_digit(const BigInt *a, const size_t k)
{
  const BiBlock *a_digits = DIGITS(a);
  if ( k < LENGTH(a) * BiBlock_bit )
      return (a_digits[k / BiBlock_bit] >> (k % BiBlock_bit)) & 1;
  else
    return SIGN(a_digits[LENGTH(a)-1]);
}

int bi_minus(BigInt *a, const BigInt *b)
{
  BiBlock a_digits[LENGTH(b)+1];
  const BiBlock *b_digits = DIGITS(b);
  BiBlock carry = 1;
  for ( size_t i = 0 ; i < LENGTH(b) ; i++ )
  {
    a_digits[i] = ~b_digits[i] + carry;
    carry = a_digits[i] == b_digits[i];
  }
  a_digits[LENGTH(b)] = carry + SIGN(b_digits[LENGTH(b)-1]) - 1;

  return bi_normalize(a, a_digits, LENGTH(b)+1);
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

int bi_add(BigInt *a, const BigInt *b, const BigInt *c)
{
  if ( LENGTH(b) < LENGTH(c) )
  {
    const BigInt *tmp = b; b = c; c = tmp;
  } // b has at least as many digits as c

  BiBlock a_digits[LENGTH(b)+1];
  const BiBlock *b_digits = DIGITS(b);
  const BiBlock *c_digits = DIGITS(c);

  bi_block_copy(a_digits, LENGTH(b)+1, b_digits, LENGTH(b));
  bi_block_add_to(a_digits, LENGTH(b)+1, c_digits, LENGTH(c));

  return bi_normalize(a, a_digits, LENGTH(b)+1);
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

int bi_sub(BigInt *a, const BigInt *b, const BigInt *c)
{
  const size_t a_length
    = 1 + (( LENGTH(b) < LENGTH(c) ) ? LENGTH(c) : LENGTH(b));
  BiBlock a_digits[a_length];
  const BiBlock *b_digits = DIGITS(b);
  const BiBlock *c_digits = DIGITS(c);

  bi_block_copy(a_digits, a_length, b_digits, LENGTH(b));
  bi_block_sub_from(a_digits, a_length, c_digits, LENGTH(c));

  return bi_normalize(a, a_digits, a_length);
}

int bi_compare(const BigInt *a, const BigInt *b)
{
  const BiBlock *a_digits = DIGITS(a);
  const BiBlock *b_digits = DIGITS(b);

  BiBlock a_sign = SIGN(a_digits[LENGTH(a)-1]);
  BiBlock b_sign = SIGN(b_digits[LENGTH(b)-1]);

  if ( a_sign == b_sign )
  {
    if ( LENGTH(a) == LENGTH(b) )
    {
        for ( size_t i = LENGTH(a) ; 0 < i ; i-- )
        {
          if ( a_digits[i-1] < b_digits[i-1] )
            return -1;
          else if ( a_digits[i-1] > b_digits[i-1] )
            return 1;
        }
        return 0;
    }
    else if ( (0 == a_sign && LENGTH(a) < LENGTH(b))
           || (1 == a_sign && LENGTH(a) > LENGTH(b)) )
      return -1;
    else
      return 1;
  }
  else if ( a_sign == 0 ) // b < 0 <= a
    return 1;
  else // a < 0 <= b
    return -1;
}

/*! Return -1 if the big int is too large */
int bi_to_int(const BigInt *a)
{
  if ( bi_log2(a) < sizeof(int) * CHAR_BIT )
    return SIMPLE(a);
  else
    return -1;
}

void bi_block_set(BiBlock *a, const size_t k, const int bit)
{
  size_t pos = k / BiBlock_bit, ofs = k % BiBlock_bit;
  a[pos] |= (BiBlock) 1 << ofs;
  a[pos] ^= (BiBlock) !bit << ofs;
}

//! Divides `a` by `d`, put the quotient back in `a` and return the remainer.
BiBlock bi_div_int(BigInt *a, BiBlock d)
{
  BiBlock *a_digits = DIGITS(a);
  BiBlock r = 0, q = 0;
  for ( size_t i = LENGTH(a) - 1 ; ; i-- )
  {
    r = a_digits[i] % d;
    a_digits[i] = a_digits[i] / d + q;
    if ( i == 0 )
      break;
    else // i > 0
    {
      q = 0;
      size_t k;
      for ( k = 1 ; k < BiBlock_bit && d >> k != 0 ; k++ )
      {
        BiBlock next_digit = a_digits[i-1] - (d << (BiBlock_bit - k));
        BiBlock carry = next_digit <= a_digits[i-1] ? 0 : 1;
        if ( (d >> k) + carry <= r )
        {
          r -= (d >> k) + carry;
          q += (BiBlock) 1 << (BiBlock_bit - k);
          a_digits[i-1] = next_digit;
        }
      }
      assert(r <= 1);
      if ( r == 1 )
      {
        q += (BiBlock) 1 << (BiBlock_bit - k);
        a_digits[i-1] -= d << (BiBlock_bit - k);
      }
    }
  }
  while ( 1 < LENGTH(a) && a_digits[LENGTH(a) - 1] == 0 )
    LENGTH(a)--;
  return r;
}

int bi_to_string(char *dest, const BigInt *a, char base)
{
  assert( base <= 36 );
  BigInt b;
  if ( -1 == bi_compare(a, bi_zero_const) )
  {
    if ( bi_minus(&b, a) )
      return 1;
    dest[0] = '-';
    dest++;
  }
  else if ( bi_copy(&b, a) )
      return 1;

  size_t n = 0;
  do
  {
    dest[n++] = bi_div_int(&b, base);
  }
  while ( !bi_is_zero(&b) );
  dest[n] = '\0';
  for ( size_t i = 0 ; 2 * i < n - 1 ; i++ )
  {
    char tmp = dest[i]; dest[i] = dest[n-1-i]; dest[n-1-i] = tmp;
  }
  for ( size_t i = 0 ; i < n ; i++ )
    dest[i] = digit_to_char(dest[i]);
  return 0;
}

int bi_from_string(BigInt *a, const char *src, const char base)
{
  assert( base <= 36 );
  const int a_neg = src[0] == '-';
  if ( a_neg ) src++;
  const size_t n = strlen(src);
  char src_[n];
  for ( size_t i = 0 ; i < n ; i++ )
  {
    if ( -1 == (src_[i] = char_to_digit(src[i])) )
      return 2;
  }
  MAX_LENGTH(a) = 6 * n / BiBlock_bit + 1;
  BiBlock a_digits[MAX_LENGTH(a)];
  size_t start = 0, length;
  for ( length = 0 ; start < n ; length++ )
  {
    if ( 0 == length % BiBlock_bit )
      a_digits[length/BiBlock_bit] = 0;
    // Divide by 2
    int r;
    for ( size_t i = start ; i < n ; i++ )
    {
      if ( i + 1 == n )
        r = src_[i] % 2;
      else if ( 1 == src_[i] % 2 )
        src_[i+1] += base;
      src_[i] /= 2;
    }
    bi_block_set(a_digits, length, r);
    while ( start < n && 0 == src_[start] )
      start++;
  }
  LENGTH(a) = (length - 1) / BiBlock_bit + 1;
  COMPOSITE(a) = a_digits;
  if ( a_neg )
  {
    return bi_minus(a, a);
  }
  else
    return bi_normalize(a, a_digits, LENGTH(a));
}

void bi_binary_string(char *dest, const BigInt *a)
{
  size_t n = bi_log2(a);
  for ( size_t i = 0 ; i < n ; i++ )
  {
    dest[i] = bi_digit(a, i) ? '1' : '0';
  }
  dest[n] = '\0';
}

char** bi_to_char_mat(
  const BigInt ***bm,
  int m,
  int n,
  int height)
{
  fputs("Unimplemented bi_to_char_mat\n", stderr);
  exit(1);
}

#undef COMPOSITE
#undef LENGTH
#undef SIMPLE
#undef MAX_LENGTH
