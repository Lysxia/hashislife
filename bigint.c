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

const BigInt bi_zero = {.digits = NULL, .len = 0};

/* Number of bits */
int bi_log2(BigInt b)
{
  int d = b.len * (bi_block_bit - 1);

  while ( d > 0 &&
          !(
            b.digits[(d - 1) / (bi_block_bit - 1)] == 0 &&
            1 << ((d - 1) % (bi_block_bit - 1)) == 0) )
    d--;

  return d;
}

int bi_digit(BigInt b, int d)
{
  if ( d < b.len * (bi_block_bit - 1) )
    return (b.digits[d / (bi_block_bit - 1)] >> (d % (bi_block_bit - 1))) & 1;
  else
    return 0;
}

int bi_iszero(BigInt b)
{
  bi_canonize(&b);
  return b.len == 0;
}

BigInt bi_copy(BigInt b)
{
  if (bi_iszero(b))
    return bi_zero;

  BigInt c = {
    .len = b.len,
    .digits = malloc(b.len * sizeof(bi_block)),
  };

  if ( c.digits == NULL )
  {
    perror("bi_copy()");
    exit(1);
  }

  memcpy(c.digits, b.digits, b.len * sizeof(bi_block));

  return c;
}

void bi_canonize(BigInt *b)
{
  while ( b->len > 0 && b->digits[b->len - 1] != 0 )
    b->len--;

  if ( b->len == 0 )
  {
    free(b->digits);
    b->digits = NULL;
  }
}

BigInt bi_power_2(int k)
{
  BigInt s = {
    .digits = calloc(k / (bi_block_bit - 1) + 1, sizeof(bi_block)),
    .len = k / (bi_block_bit - 1) + 1
  };

  if (s.digits == NULL)
  {
    perror("bi_power_2()");
    exit(1);
  }

  s.digits[k / (bi_block_bit - 1)] |= (bi_block) 1 << (k % (bi_block_bit - 1));

  return s;
}

void bi_smooth(bi_block *digits, int start, int end);

BigInt bi_plus_int(BigInt b, int i)
{
  BigInt s = {
    .digits = malloc((b.len + 1) * sizeof(bi_block)),
    .len = b.len + 1,
  };

  if (s.digits == NULL)
  {
    perror("bi_plus_int()");
    exit(1);
  }

  if (b.len == 0)
  {
    s.digits[0] = i;
    bi_canonize(&s);

    return s;
  }

  memcpy(s.digits, b.digits, b.len * sizeof(bi_block));
  s.digits[b.len] = 0;
  s.digits[0] += i;
  bi_smooth(s.digits, 0, b.len);
  bi_canonize(&s);

  return s;
}

BigInt *bi_add_to(BigInt *a, BigInt b)
{
  if (bi_iszero(*a) && bi_iszero(b))
  {
    free(a->digits);
    *a = bi_zero;
    return a;
  }

  if (a->digits == NULL)
    a->digits = malloc((b.len + 1) * sizeof(bi_block));
  else
  {
    if (a->len <= b.len)
      a->len = b.len + 1;
    else
      a->len++;

    a->digits = realloc(a->digits, a->len * sizeof(bi_block));
  }

  a->digits[a->len-1] = 0;

  int i;
  for (i = 0 ; i < b.len ; i++)
    a->digits[i] += b.digits[i];

  bi_smooth(a->digits, 0, a->len);
  bi_canonize(a);

  return a;
}

void bi_smooth(bi_block *digits, int start, int len)
{
  int i;
  for (i = 0 ; i < len ; i++)
  {
    digits[start+i+1] += (digits[start+i] >> (bi_block_bit - 1)) & 1;
    digits[start+i] &= (bi_block) (-1) ^ ((bi_block) 1 << (bi_block_bit - 1));
  }
}

int bi_to_int(BigInt b)
{
  return b.len ? b.digits[0] : 0;
}

BigInt bi_from_int(int i)
{
  BigInt s = {
    .digits = malloc(sizeof(bi_block)),
    .len = 1,
  };

  s.digits[0] = i;

  return s;
}

void bi_free(BigInt b)
{
  free(b.digits);
}

void bi_print(BigInt b)
{
  int d;
  for (d = 0 ; d < (bi_block_bit - 1) * b.len ; d++)
  {
    putchar('0' + bi_digit(b, d));
  }
  putchar('\n');
}

void bi_test()
{
  const int len = 3;
  BigInt b = {
    .digits = malloc(len * sizeof(int)),
    .len = len,
  };

  int i;

  for (i = 0 ; i < len ; i++)
    b.digits[i] = (bi_block) (-1) ^ ((bi_block) 1 << (bi_block_bit - 1));

  bi_print(b);

  BigInt c = bi_plus_int(b, 010000000000);

  bi_print(c);

  bi_free(b);
  bi_free(c);
}
