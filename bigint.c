#include <stdlib.h>
#include <stdio.h>
#include "bigint.h"

const BigInt bi_zero = {.digits = NULL, .len = 0};

int bi_length(BigInt b)
{
  return b.len;
}

int bi_digit(BigInt b, int d)
{
  return 0 <= d && d < b.len ? (b.digits[d / 31] >> (d % 31)) & 1 : 0;
}

int bi_iszero(BigInt b)
{
  return bi_canonize(&b)->len == 0;
}

BigInt bi_copy(BigInt b)
{
  if (bi_iszero(b))
    return bi_zero;

  BigInt c = {
    .len = b.len,
    .digits = malloc(1 + (b.len - 1) / 31)
  };

  memcpy(c.digits, b.digits, (1 + (b.len - 1) / 31) * sizeof(int));

  return c;
}

int bi_flip(BigInt b, int d)
{
  return b.digits[d / 31] ^= (1 << (d % 31));
}

BigInt *bi_canonize(BigInt *b)
{
  while (b->len > 0 && !bi_digit(*b, b->len - 1)) b->len--;

  return b;
}

BigInt bi_power_2(int k)
{
  BigInt s = {
    .digits = calloc(1 + k / 31, sizeof(int)),
    .len = k + 1
  };

  if (s.digits == NULL)
  {
    printf("in bi_power_2(%d): Not enough memory\n", k);
    exit(1);
  }

  s.digits[k / 31] |= 1 << (k % 31);

  return s;
}

BigInt bi_plus_int(BigInt b, int i)
{
  BigInt s = {
    .digits = malloc((2 + b.len / 31) * sizeof(int)),
    .len = 0
  };

  if (s.digits == NULL)
  {
    printf("in bi_plus_int(...): Not enough memory\n");
    exit(1);
  }

  if (b.len == 0)
  {
    s.digits[0] = i;
    
    for (s.len = 31 ; s.len > 0 && !bi_digit(s, s.len - 1) ; s.len--) {}
    return s;
  }

  int k;

  s.digits[0] = i;

  for (k = 0, s.len = 0 ; s.len < b.len ; k++, s.len += 31)
  {
    s.digits[k] += b.digits[k];
    s.digits[k + 1] = (s.digits[k] >> 31) & 1;
    s.digits[k] ^= 1 << 31;
  }

  s.len++;

  while (s.len > 0 && !bi_digit(s, s.len - 1)) s.len--;

  return s;
}

int bi_to_int(BigInt b)
{
  if (b.len == 0)
    return 0;

  if (b.len > 31)
    b.len = 31;

  return b.digits[0] & ((1 << b.len) - 1);
}

void bi_free(BigInt b)
{
  free(b.digits);
}

void bi_print(BigInt b)
{
  int d;
  for (d = 0 ; d < b.len ; d++)
  {
    putchar(bi_digit(b, d) ? '1' : '0');
  }
  putchar('\n');
}

void bi_test()
{
  const int len = 3;
  BigInt b = {
    .digits = malloc(len * sizeof(int)),
    .len = len * 31,
  };

  int i;

  for (i = 0 ; i < len ; i++)
    b.digits[i] = (-1 ^ (1 << 31));

  while (!bi_digit(b, b.len - 1)) b.len--;

  bi_print(b);

  BigInt c = bi_plus_int(b, 010000000000);

  bi_print(c);

  bi_free(b);
  bi_free(c);
}
