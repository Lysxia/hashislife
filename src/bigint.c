#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include "bigint.h"

// Unsigned type for bitwise manipulation
// We use max width integer type
typedef uintmax_t bi_block;

const bi_block bi_block_max = UINTMAX_MAX;

// Number of bits in the type bi_block
const int bi_block_bit = CHAR_BIT * sizeof(bi_block);

struct BigInt {
  bi_block *digits;
  int       len;
};

// The leading digit/block of results should always be non-zero,
// except for zero in which case there are no digits,
// the following function ensures this invariant

void bi_canonize(BigInt *b);

BigInt *bi_new(int len)
{
  BigInt *n = malloc(sizeof(BigInt));

  if ( !n )
    return NULL;

  if ( len > 0 )
  {
    n->digits = malloc(len * sizeof(bi_block));
    if ( !n->digits )
    {
      free(n);
      return NULL;
    }
  }

  n->len = len;

  return n;
}

const BigInt  bi_zero_      = { .digits = NULL, .len = 0 },
             *bi_zero_const = &bi_zero_;

BigInt *bi_zero(void)
{
  BigInt *zero = malloc(sizeof(BigInt));

  if ( !zero )
  {
    perror("bi_zero()");
    exit(1);
  }

  *zero = bi_zero_;

  return zero;
}

// Number of bits
// floor(log(bi)) + 1
int bi_log2(const BigInt *b)
{
  int d = b->len * bi_block_bit;

  while ( d > 0 &&
            ((b->digits[(d - 1) / bi_block_bit]) &
            ((bi_block) 1 << ((d - 1) % bi_block_bit)))
              == 0 )
    d--;

  return d;
}

int bi_slice(const BigInt *b, int c)
{
  int pos = c / bi_block_bit, ofs = c % bi_block_bit;

  if ( pos >= b->len )
    return 0;
  else if ( pos == b->len - 1 || bi_block_bit - (unsigned) ofs > CHAR_BIT * sizeof(int) )
    return b->digits[pos] >> ofs & INT_MAX;
  else
    return (b->digits[pos] >> ofs | b->digits[pos+1] << (bi_block_bit - ofs)) & INT_MAX;
}

int bi_digit(const BigInt *b, int d)
{
  if ( d < b->len * bi_block_bit )
    return (b->digits[d / bi_block_bit] >> (d % bi_block_bit)) & 1;
  else
    return 0;
}

int bi_iszero(const BigInt *b)
{
  return b->len == 0;
}

BigInt *bi_copy(const BigInt *b)
{
  if ( bi_iszero(b) )
    return bi_zero();
  else
  {
    BigInt *c = bi_new(b->len);

    if ( !c )
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

  if ( b->len == 0 && b->digits )
  {
    free(b->digits);
    b->digits = NULL;
  }
}

BigInt *bi_power_2(int k)
{
  const int len_ = k / bi_block_bit + 1;
  BigInt *s = bi_new(len_);

  if ( !s )
  {
    perror("bi_power_2()");
    exit(1);
  }

  memset(s->digits, 0, (len_ - 1) * sizeof(bi_block));

  s->digits[len_-1] = (bi_block) 1 << (k % bi_block_bit);

  return s;
}

BigInt *bi_plus_int(const BigInt *b, int i)
{
  BigInt *s = bi_new(b->len + 1);

  if ( !s )
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

  int k;

  for ( k = 0 ; k < b->len ; k++ )
  {
    if ( k == 0 )
    {
      s->digits[0] = b->digits[0] + i;
      if ( s->digits[0] >= b->digits[0] )
        break;
    }
    else
    {
      s->digits[k] = b->digits[k] + 1;
      if ( s->digits[k] != 0 )
        break;
    }
  }

  if ( k == b->len )
  {
    s->digits[b->len] = 1;
  }
  else
  {
    for ( k++ ; k < b->len ; k++ )
      s->digits[k] = b->digits[k];
    s->len = b->len;
  }

  return s;
}

// b - 2^e.
// If the result is positive, then return a BigInt and set *neg to 0
// If the result is negative, return 0 and set *neg to min(MAX_INT, 2^e - b)
BigInt *bi_minus_pow(const BigInt *b, int e, int *neg)
{
  if ( b->len == 0 )
  {
    *neg = (unsigned) e < CHAR_BIT * sizeof(int) - 1 ? 1 << e : INT_MAX;
    return bi_zero();
  }

  int loc = e / bi_block_bit, ofs = e % bi_block_bit;

  if ( loc > b->len || (loc == b->len && ofs != 0) )
  {
    *neg = INT_MAX;
    return bi_zero();
  }

  BigInt *c = bi_copy(b);

  int block_pos;

  if ( loc < b->len )
  {
    for ( block_pos = loc ; block_pos < c->len ; block_pos++ )
    {
      if ( block_pos == loc )
      {
        c->digits[block_pos] -= (bi_block) 1 << ofs;
        if ( c->digits[block_pos] < b->digits[block_pos] )
          break;
      }
      else
      {
        c->digits[block_pos]--;
        if ( c->digits[block_pos] != bi_block_max )
          break;
      }
    }
  }
  else
    block_pos = c->len;

  // Overflow
  if ( block_pos == c->len )
  {
    int i;
    for ( i = c->len ; i > 0 ; i-- )
      if ( c->digits[i-1] != bi_block_max )
        break;

    bi_block neg_ = ~c->digits[0] + 1;
    *neg = i <= 1 && neg_ < INT_MAX ? neg_ : INT_MAX;

    bi_free(c);
    return bi_zero();
  }
  else
  {
    bi_canonize(c);
    *neg = 0;
    return c;
  }
}

BigInt *bi_add(const BigInt *a, const BigInt *b)
{
  if ( bi_iszero(a) && bi_iszero(b) )
    return bi_zero();
  else if ( a->len < b->len )
  {
    const BigInt *c = b;
    b = a;
    a = c;
  }

  BigInt *c = bi_new(a->len + 1);

  if ( !c )
  {
    perror("bi_add()");
    exit(1);
  }

  int i;
  for ( i = 0, c->digits[0] = 0 ; i < b->len ; i++ )
  {
    c->digits[i] += a->digits[i] + b->digits[i];
    c->digits[i+1] = c->digits[i] < a->digits[i] || c->digits[i] < b->digits[i];
  }
  for ( ; i < a->len ; i++ )
  {
    c->digits[i] += a->digits[i];
    c->digits[i+1] = c->digits[i] < a->digits[i];
  }

  bi_canonize(c);

  return c;
}

int bi_to_int(const BigInt *b)
{
  return b->len ? b->digits[0] : 0;
}

BigInt *bi_from_int(int i)
{
  if ( i == 0 )
    return bi_zero();
  else
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
}

BigInt *bi_mult_int(const BigInt *b, int n);

BigInt *bi_from_string(const char *c, int base)
{
  if ( base < 2 || 10 < base )
    return NULL;

  BigInt *acc = bi_zero();

  int i;

  for ( i = 0 ; '0' <= c[i] && c[i] < '0' + base ; i += (c[i+1] == ',') + 1 )
  {
    BigInt *tmp = bi_mult_int(acc, base);
    bi_free(acc);
    acc = bi_plus_int(tmp, c[i] - '0');
    bi_free(tmp);
  }

  return acc;
}

BigInt *bi_mult_int(const BigInt *b, int n)
{
  BigInt *acc = bi_zero();

  int i;
  for ( i = CHAR_BIT * sizeof(int) - 2 ; i >= 0 ; i-- )
  {
    BigInt *tmp = bi_add(acc, acc);
    bi_free(acc);

    if ( n & 1 << i )
    {
      acc = bi_add(tmp, b);
      bi_free(tmp);
    }
    else
      acc = tmp;
  }

  return acc;
}

void bi_free(BigInt *b)
{
  if ( b->digits )
    free(b->digits);
  free(b);
}

void bi_print(const BigInt *b)
{
  int d;
  for ( d = 0 ; d < bi_log2(b) ; d++ )
  {
    if ( d > 0 && !(d % 8) )
      putchar(' ');
    putchar('0' + bi_digit(b, d));
  }
  putchar('\n');
}

void bi_test()
{
#if 0
  const int len = 3;
  BigInt *b = bi_new(len);

  int i;

  b->digits[0] = -1 ^ 36;
  for ( i = 1 ; i < len ; i++ )
    b->digits[i] = -1;

  bi_canonize(b);
  printf("%d\n", b->len);

  bi_print(b);

  BigInt *c = bi_plus_int(b, 010000000000);

  bi_print(c);

  int neg;

  BigInt *d = bi_minus_pow(b, 192, &neg);

  bi_print(d);
  printf("%d\n", neg);

  bi_free(b);
  bi_free(c);
  bi_free(d);
#else
  BigInt *b = bi_from_string("600,40100200,40100200,40100200", 8);

  bi_print(b);
  bi_free(b);
#endif
}
