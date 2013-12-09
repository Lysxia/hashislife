#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bigint.h"

const BigInt bi_zero = {.digits = NULL, .len = 0};

/* Number of bits */
int bi_log2(BigInt b)
{
  int d = b.len * 31;

  while (d > 0 && !(b.digits[(d - 1) / 31] & (1 << ((d - 1) % 31)))) d--;

  return d;
}

int bi_digit(BigInt b, int d)
{
  return d < b.len * 31 ? (b.digits[d / 31] >> (d % 31)) & 1 : 0;
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
    .digits = malloc(b.len * sizeof(int)),
  };

  memcpy(c.digits, b.digits, b.len * sizeof(int));

  return c;
}

int bi_flip(BigInt b, int d)
{
  return b.digits[d / 31] ^= (1 << (d % 31));
}

BigInt *bi_canonize(BigInt *b)
{
  while (b->len > 0 && !b->digit[b->len - 1]) b->len--;

  return b;
}

BigInt bi_power_2(int k)
{
  BigInt s = {
    .digits = calloc(k / 31 + 1, sizeof(int)),
    .len = k / 31 + 1
  };

  if (s.digits == NULL)
  {
    printf("in bi_power_2(%d): Not enough memory\n", k);
    exit(1);
  }

  s.digits[k / 31] |= 1 << (k % 31);

  return s;
}

int *bi_smooth(int *digits, int start, int end);

BigInt bi_plus_int(BigInt b, int i)
{
  const int size = b.len + 1;

  BigInt s = {
    .digits = malloc(size * sizeof(int)),
    .len = size,
  };

  if (s.digits == NULL)
  {
    printf("in bi_plus_int(...): Not enough memory\n");
    exit(1);
  }

  if (b.len == 0)
  {
    s.digits[0] = i;
    bi_canonize(&s)
    
    return s;
  }

  s.digits[size - 1] = 0;
  memcpy(s.digits, d.digits, b.len * sizeof(int));
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
    a->digits = NULL;
    return *a = bi_zero;
  }

  if (a->len <= b.len)
  {
    if (a->digits == NULL)
      a->digits = malloc((b.len + 1) * sizeof(int));
    else
      a->digits = realloc(a->digits, (b.len + 1) * sizeof(int));
    a->len = b.len+1;
  }

  int i;
  for (i = 0 ; i < b.len ; i++)
    a->digits[i] += b.digits[i];

  a->digits[b.len] = 0;

  bi_smooth(a->digits, 0, b.len);
  bi_canonize(a);

  return *a;
}

int *bi_smooth(int *digits, int start, int len)
{
  int i;
  for (i = 0 ; i < len ; i++)
  {
    digits[start+i+1] += (digits[start+i] >> 31) & 1;
    digits[start+i] &= -1 ^ (1 << 31);
  }

  return digits;
}

int bi_to_int(BigInt b)
{
  return b.len ? b.digits[0] : 0;
}

void bi_free(BigInt b)
{
  free(b.digits);
}

void bi_print(BigInt b)
{
  int d;
  for (d = 0 ; d < 31 * b.len ; d++)
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
    .len = len,
  };

  int i;

  for (i = 0 ; i < len ; i++)
    b.digits[i] = (-1 ^ (1 << 31));

  bi_print(b);

  BigInt c = bi_plus_int(b, 010000000000);

  bi_print(c);

  bi_free(b);
  bi_free(c);
}
