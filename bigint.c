#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include "bigint.h"

// Unsigned type for bitwise manipulation
// We use max width integer type
typedef uintmax_t bi_block;

// Number of bits in the type bi_block
const int bi_block_bit = CHAR_BIT * sizeof(bi_block);

struct BigInt {
  bi_block *digits;
  int len;
};

// The leading digit/block of results should always be non-zero,
// except for zero in which case there are no digits,
// the following function ensures this invariant

void bi_canonize(BigInt *b);

BigInt *bi_new(int len)
{
  BigInt *n = malloc(sizeof(BigInt));

  if ( n == NULL )
    return NULL;

  if ( len > 0 )
  {
    n->digits = malloc(len * sizeof(bi_block));
    if ( n->digits == NULL )
    {
      free(n);
      return NULL;
    }
  }

  n->len = len;

  return n;
}

const BigInt bi_zero_ = {.digits = NULL, .len = 0}, *bi_zero = &bi_zero_;

// Number of bits
// floor(log(bi)) + 1
int bi_log2(const BigInt *b)
{
  int d = b->len * (bi_block_bit - 1);

  while ( d > 0 &&
            ((b->digits[(d - 1) / (bi_block_bit - 1)]) &
            ((bi_block) 1 << ((d - 1) % (bi_block_bit - 1))))
              == 0 )
    d--;

  return d;
}

int bi_digit(const BigInt *b, int d)
{
  if ( d < b->len * (bi_block_bit - 1) )
    return (b->digits[d / (bi_block_bit - 1)] >> (d % (bi_block_bit - 1))) & 1;
  else
    return 0;
}

int bi_iszero(const BigInt *b)
{
  return b->len == 0;
}

BigInt *bi_copy(const BigInt *b)
{
  if (bi_iszero(b))
  {
    return bi_new(0);
  }
  else
  {
    BigInt *c = bi_new(b->len);

    if ( c == NULL )
    {
      perror("bi_copy()");
      return NULL;
    }

    memcpy(c->digits, b->digits, b->len * sizeof(bi_block));

    return c;
  }
}

void bi_canonize(BigInt *b)
{
  while ( b->len > 0 && b->digits[b->len - 1] == 0 )
    b->len--;

  if ( b->len == 0 && b->digits != NULL )
  {
    free(b->digits);
    b->digits = NULL;
  }
}

BigInt *bi_power_2(int k)
{
  BigInt *s = bi_new(k / (bi_block_bit - 1) + 1);

  if (s == NULL)
  {
    perror("bi_power_2()");
    exit(1);
  }

  memset(s->digits, 0, (s->len - 1) * sizeof(bi_block));

  s->digits[k / (bi_block_bit - 1)] |= (bi_block) 1 << (k % (bi_block_bit - 1));

  return s;
}

// Propagates carries from start to start+len,
// assumes no block is all 1's (carry included)
void bi_smooth(bi_block *const digits, const int start, const int len);

BigInt *bi_plus_int(const BigInt *b, int i)
{
  BigInt *s = bi_new(b->len + 1);

  if ( s == NULL )
  {
    perror("bi_plus_int()");
    exit(1);
  }

  if ( b->len == 0 )
  {
    s->digits[0] = i;
    bi_canonize(s);

    return s;
  }

  memcpy(s->digits, b->digits, b->len * sizeof(bi_block));
  s->digits[b->len] = 0;
  s->digits[0] += i;
  bi_smooth(s->digits, 0, b->len);
  bi_canonize(s);

  return s;
}

#define MAX(a,b) (((a) < (b))?(b):(a))

void bi_add_to(BigInt *a, const BigInt *b)
{
  if ( bi_iszero(a) && bi_iszero(b) )
    return;

  a->len = MAX(a->len, b->len) + 1;
  a->digits = realloc(a->digits, a->len * sizeof(bi_block));
  a->digits[a->len-1] = 0;

  int i;
  for ( i = 0 ; i < b->len ; i++ )
    a->digits[i] += b->digits[i];

  bi_smooth(a->digits, 0, a->len);
  bi_canonize(a);
}

void bi_smooth(bi_block *digits, const int start, const int len)
{
  int i;
  for ( i = 0 ; i < len ; i++ )
  {
    digits[start+i+1] += (digits[start+i] >> (bi_block_bit - 1)) & 1;
    digits[start+i] &= (bi_block) (-1) ^ ((bi_block) 1 << (bi_block_bit - 1));
  }
}

int bi_to_int(const BigInt *b)
{
  return b->len ? b->digits[0] : 0;
}

BigInt *bi_from_int(int i)
{
  BigInt *s = bi_new(1);
 
  if ( s == NULL )
  {
    perror("bi_to_int()");
    exit(1);
  }

  s->digits[0] = i;

  return s;
}

void bi_free(BigInt *b)
{
  free(b->digits);
  free(b);
}

void bi_print(BigInt *b)
{
  int d;
  putchar('0');
  for (d = 0 ; d < bi_log2(b) ; d++)
  {
    putchar('0' + bi_digit(b, d));
  }
  putchar('\n');
}

void bi_test()
{
  const int len = 3;
  BigInt *b = bi_new(len);

  int i;

  for (i = 0 ; i < len ; i++)
    b->digits[i] = (bi_block) (-1) ^ ((bi_block) 1 << (bi_block_bit - 1));

  bi_canonize(b);
  printf("%d\n", b->len);

  bi_print(b);

  BigInt *c = bi_plus_int(b, 010000000000);

  bi_print(c);

  bi_free(b);
  bi_free(c);
}
