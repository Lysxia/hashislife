#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bigint.h"
#include "definitions.h"

#define C_ARRAY(a) (a)->digits.composite.array
#define C_LENGTH(a) (a)->digits.composite.len
#define SIMPLE(a) (a)->digits.simple
#define MAX_LEN(a) (a)->max_len
#define IS_SIMPLE(a) 0 == (a)->max_len
#define IS_COMPOSITE(a) 0 < (a)->max_len

const size_t BiBlock_bit = CHAR_BIT * sizeof(BiBlock);
const BiBlock BiBlock_max = UINTMAX_MAX;

const BigInt bi_zero_const_ = {
  .digits = { .simple = 0 },
  .max_len = 0
};
const BigInt * const bi_zero_const = &bi_zero_const_;

void bi_simple(BigInt *a, const BiBlock n)
{
  MAX_LEN(a) = 0;
  SIMPLE(a) = n;
}

void bi_zero(BigInt *a)
{
  if ( IS_COMPOSITE(a) )
    free(C_ARRAY(a));
  MAX_LEN(a) = 0;
  SIMPLE(a) = 0;
}

/* Return `0` if success, return `1` if failure (without modifying `a`). */
int bi_normalize(BigInt *a)
{
  if ( IS_COMPOSITE(a) )
  {
    if ( C_LENGTH(a) < 2 )
    {
      BiBlock tmp = C_ARRAY(a)[0];
      free(C_ARRAY(a));
      MAX_LEN(a) = 0;
      SIMPLE(a) = tmp;
    }
    else if ( C_LENGTH(a) < MAX_LEN(a) )
    {
      BiBlock *tmp = realloc(C_ARRAY(a), C_LENGTH(a) * sizeof(BiBlock));
      if ( NULL == tmp )
        return 1;
      C_ARRAY(a) = tmp;
      MAX_LEN(a) = C_LENGTH(a);
    }
  }
  return 0;
}

int bi_is_zero(const BigInt *a)
{
  return ( IS_SIMPLE(a) ? 0 == SIMPLE(a) : 0 == C_LENGTH(a) );
}

/*! 0 when `a` is zero */
size_t bi_log2(const BigInt *a)
{
  if ( IS_COMPOSITE(a) && 0 == C_LENGTH(a) )
    return 0;

  size_t d = BiBlock_bit, nb_prec_blocks;
  BiBlock last_block;

  if ( IS_SIMPLE(a) )
  {
    nb_prec_blocks = 0;
    last_block = SIMPLE(a);
  }
  else
  {
    nb_prec_blocks = C_LENGTH(a)-1;
    last_block = C_ARRAY(a)[C_LENGTH(a)-1];
  }

  while ( d > 0 && 0 == (1 & (last_block >> (d - 1))) )
    d--;

  return d + nb_prec_blocks * BiBlock_bit;
}

int bi_copy(BigInt *a, const BigInt *b)
{
  if ( IS_SIMPLE(b) )
  {
    MAX_LEN(a) = 0;
    SIMPLE(a) = SIMPLE(b);
  }
  else
  {
    BiBlock *array = malloc(C_LENGTH(b) * sizeof(*array));
    if ( NULL == array )
      return 1;
    memcpy(array, C_ARRAY(b), C_LENGTH(b) * sizeof(*array));
    MAX_LEN(a) = C_LENGTH(a) = C_LENGTH(b);
    C_ARRAY(a) = array;
  }
  return 0;
}

int bi_slice(const BigInt *a, const size_t k)
{
  size_t pos = k / BiBlock_bit, ofs = k % BiBlock_bit;

  if ( IS_SIMPLE(a) )
  {
    if ( 0 == pos )
      return (int) ((unsigned) (SIMPLE(a) >> ofs) & INT_MAX);
    else
      return 0;
  }
  else if ( pos == C_LENGTH(a) - 1
         || BiBlock_bit - ofs > CHAR_BIT * sizeof(int) )
    return (C_ARRAY(a)[pos] >> ofs) & INT_MAX;
  else if ( pos < C_LENGTH(a) - 1 )
    return (C_ARRAY(a)[pos+1] << (BiBlock_bit - ofs) | C_ARRAY(a)[pos] >> ofs)
      & INT_MAX;
  else
    return 0;
}

int bi_digit(const BigInt *a, const size_t k)
{
  if ( IS_SIMPLE(a) && k < BiBlock_bit )
      return (SIMPLE(a) >> k) & 1;
  else if ( IS_COMPOSITE(a) && k < C_LENGTH(a) * BiBlock_bit )
      return (C_ARRAY(a)[k / BiBlock_bit] >> (k % BiBlock_bit)) & 1;
  else
    return 0;
}

const BiBlock *bi_digits(size_t *len, const BigInt *a)
{
  if ( IS_SIMPLE(a) )
  {
    *len = 1;
    return &SIMPLE(a);
  }
  else
  {
    *len = C_LENGTH(a);
    return C_ARRAY(a);
  }
}

int bi_add(BigInt *a, const BigInt *b, const BigInt *c)
{
  size_t b_len, c_len;
  const BiBlock *b_digits, *c_digits;

  b_digits = bi_digits(&b_len, b);
  c_digits = bi_digits(&c_len, c);

  if ( b_len < c_len )
  {
    size_t tmp = b_len; b_len = c_len; c_len = tmp;
    const BiBlock *tmp_d = b_digits; b_digits = c_digits; c_digits = tmp_d;
  } // b now has at least as many digits as c

  BiBlock *a_digits = malloc((b_len + 1) * sizeof(*a_digits));
  if ( NULL == a_digits )
    return 1;
  size_t a_len;

  a_digits[0] = 0;
  // overflow check
#define ADD_TO_OF_CHECK(a,b,carry) if ( (a+=b) < b ) carry = 1
  for ( size_t i = 0 ; i < b_len ; i++ )
  {
    a_digits[i+1] = 0;
    ADD_TO_OF_CHECK(a_digits[i], b_digits[i], a_digits[i+1]);
    if ( i < c_len )
      ADD_TO_OF_CHECK(a_digits[i], c_digits[i], a_digits[i+1]);
  }

  a_len = 0 == a_digits[b_len] ? b_len : b_len + 1;

  if ( a_len <= 1 )
  {
    MAX_LEN(a) = 0;
    SIMPLE(a) = 0 == a_len ? 0 : a_digits[0];
    free(a_digits);
  }
  else
  {
    MAX_LEN(a) = b_len + 1;
    C_ARRAY(a) = a_digits;
    C_LENGTH(a) = a_len;
  }
  return 0;
#undef ADD_TO_OF_CHECK
}

/*! Return -1 if the big int is too large */
int bi_to_int(const BigInt *a)
{
  if ( bi_log2(a) < sizeof(int) * CHAR_BIT )
    return SIMPLE(a);
  else
    return -1;
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

void bi_block_set(BiBlock *a, const size_t k, const int bit)
{
  size_t pos = k / BiBlock_bit, ofs = k % BiBlock_bit;
  a[pos] |= (BiBlock) 1 << ofs;
  a[pos] ^= (BiBlock) !bit << ofs; 
}

//! Divides `a` by `d`, put the quotient back in `a` and return the remainer.
BiBlock bi_div_int(BigInt *a, BiBlock d)
{
  if ( IS_SIMPLE(a) )
  {
    BiBlock r = SIMPLE(a) % d;
    SIMPLE(a) /= d;
    return r;
  }
  else
  {
    BiBlock r = 0, q = 0;
    for ( size_t i = C_LENGTH(a) - 1 ; ; i-- )
    {
      r = C_ARRAY(a)[i] % d;
      C_ARRAY(a)[i] = C_ARRAY(a)[i] / d + q;
      if ( i == 0 )
        break;
      else // i > 0
      {
        q = 0;
        size_t k;
        for ( k = 1 ; k < BiBlock_bit && d >> k != 0 ; k++ )
        {
          BiBlock next_digit = C_ARRAY(a)[i-1] - (d << (BiBlock_bit - k));
          BiBlock carry = next_digit <= C_ARRAY(a)[i-1] ? 0 : 1;
          if ( (d >> k) + carry <= r )
          {
            r -= (d >> k) + carry;
            q += (BiBlock) 1 << (BiBlock_bit - k);
            C_ARRAY(a)[i-1] = next_digit;
          }
        }
        assert(r <= 1);
        if ( r == 1 )
        {
          q += (BiBlock) 1 << (BiBlock_bit - k);
          C_ARRAY(a)[i-1] -= d << (BiBlock_bit - k);
        }
      }
    }
    while ( 0 < C_LENGTH(a) && C_ARRAY(a)[C_LENGTH(a) - 1] == 0 )
      C_LENGTH(a)--;
    return r;
  }
}

int bi_to_string(char *dest, const BigInt *a, char base)
{
  assert( base <= 36 );
  BigInt b;
  if ( bi_copy(&b, a) )
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

int bi_from_string(BigInt *a, const char *dest, const char base)
{
  assert( base <= 36 );
  size_t n = strlen(dest);
  char dest_[n];
  for ( size_t i = 0 ; i < n ; i++ )
  {
    if ( -1 == (dest_[i] = char_to_digit(dest[i])) )
      return 2;
  }
  size_t max_len = 6 * n / BiBlock_bit + 1;
  BiBlock *array = malloc(max_len * sizeof(*array));
  if ( NULL == array )
    return 1;
  C_ARRAY(a) = array;
  MAX_LEN(a) = max_len;
  size_t start = 0, length;
  for ( length = 0 ; start < n ; length++ )
  {
    if ( 0 == length % BiBlock_bit )
      array[length/BiBlock_bit] = 0;
    // Divide by 2
    int r;
    for ( size_t i = start ; i < n ; i++ )
    {
      if ( i + 1 == n )
        r = dest_[i] % 2;
      else if ( 1 == dest_[i] % 2 )
        dest_[i+1] += base;
      dest_[i] /= 2;
    }
    bi_block_set(array, length, r);
    while ( start < n && 0 == dest_[start] )
      start++;
  }
  C_LENGTH(a) = (length - 1) / BiBlock_bit + 1;
  return 0;
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

#undef C_ARRAY
#undef C_LENGTH
#undef SIMPLE
#undef MAX_LEN
