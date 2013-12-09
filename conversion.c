#include <stdio.h>
#include "bigint.h"
#include "hashtbl.h"
#include "hashlife.h"
#include "conversion.h"

Quad *matrix_to_quad_(Hashtbl *htbl, int **matrix, int mlen, int nlen,
                      int mmin, int nmin, int d, int s);

Quad *matrix_to_quad(Hashtbl *htbl, int **matrix, int mlen, int nlen)
{
  int side = 2, d = 0;

  while (side < mlen || side < nlen)
  {
    side <<= 1;
    d++;
  }

  return matrix_to_quad_(htbl, matrix,mlen,nlen, 0, 0, d, side);
}

Quad *matrix_to_quad_(Hashtbl *htbl, int **matrix, int mlen, int nlen,
                      int mmin, int nmin, int d, int s)
{
  if (mmin >= mlen || nmin >= nlen)
  {
    return dead_space(htbl, d);
  }
  else if (d == 0)
  {
    int i, j, acc = 0;
    for (i = 0 ; i < 2 ; i++)
      for (j = 0 ; j < 2 ; j++)
      {
        if (mmin + i >= 0 && mmin + i < mlen &&
            nmin + j >= 0 && nmin + j < nlen)
          acc |= !!matrix[mmin+i][nmin+j] << (3 - 2 * i - j);
      }

    return leaf(acc);
  }
  else
  {
    Quad *quad[4];

    int i;

    s /= 2;

    for (i = 0 ; i < 4 ; i++)
      quad[i] = matrix_to_quad_(htbl, matrix,mlen,nlen,
          mmin + (i & 2 ? s : 0),
          nmin + (i & 1 ? s : 0),
          d-1, s);

    return cons_quad(htbl, quad, d);
  }
}

void quad_to_matrix_(int **matrix, int m, int n,
                     BigInt mmin, BigInt nmin,
                     BigInt mmax, BigInt nmax,
                     Quad *q, BigInt *two_, int d);
int ge_exponent(BigInt b, int e);
void split_(BigInt min, BigInt max, BigInt two_e, int e, BigInt min_[2], BigInt max_[2], int *m2,
            int *trunc_min, int *trunc_max);
BigInt truncate(BigInt b, int e);

void quad_to_matrix(int **matrix, int m, int n,
                    int mlen, int nlen,
                    BigInt mmin, BigInt nmin, Quad *q)
{
  //printf("*** %d %d : %d %d : %d %d\n", mlen, nlen, bi_to_int(mmin), bi_to_int(nmin), q->depth, 1 << q->depth+1);
  BigInt mmax = bi_plus_int(mmin, mlen);
  BigInt nmax = bi_plus_int(nmin, nlen);
  BigInt two_[q->depth+1];

  int e;
  for (e = 0 ; e <= q->depth ; e++)
    two_[e] = bi_power_2(e);

  bi_canonize(&mmax);
  bi_canonize(&nmax);

  quad_to_matrix_(matrix, m, n,
    mmin, nmin, mmax, nmax,
    q, two_, q->depth);
}

int count = 0;

void quad_to_matrix_(int **matrix, int m, int n,
                         BigInt mmin, BigInt nmin,
                         BigInt mmax, BigInt nmax,
                         Quad *q, BigInt *two_, int d)
{
  //printf("%d %d : %d %d : %d %d\n", bi_to_int(mmin), bi_to_int(mmax), bi_to_int(nmax), bi_to_int(nmin), q->depth, 1 << q->depth+1);
  if (d == 0)
  {
    int i, j;
    int mmin_ = bi_to_int(mmin),
        nmin_ = bi_to_int(nmin);
    int mmax_ = bi_to_int(mmax),
        nmax_ = bi_to_int(nmax);
    int mlen = mmax_ - mmin_;
    int nlen = nmax_ - nmin_;

    for (i = 0 ; i < mlen ; i++)
      for (j = 0 ; j < nlen ; j++)
        matrix[m+i][n+j] = q->node.l.map[2*(mmin_+i)+(nmin_+j)];
  }
  else if (bi_iszero(mmax) || bi_iszero(nmax))
  {
    return;
  }
  else
  {
    int m_[2] = {m, m}, n_[2] = {n, n};
    BigInt mmin_[2], nmin_[2], mmax_[2], nmax_[2];
    int trunc_mmin, trunc_nmin, trunc_mmax, trunc_nmax;

    split_(mmin, mmax, two_[d], d, mmin_, mmax_, &m_[1], &trunc_mmin, &trunc_mmax);
    split_(nmin, nmax, two_[d], d, nmin_, nmax_, &n_[1], &trunc_nmin, &trunc_nmax);

    int i;

    for (i = 0 ; i < 4 ; i++)
    {
      const int x = i >> 1, y = i & 1;

      quad_to_matrix_(matrix, m_[x], n_[y],
        mmin_[x], nmin_[y],
        mmax_[x], nmax_[y],
        q->node.n.sub[i], two_, d-1);
    }

    if (trunc_mmax)
    {
      bi_free(mmax_[1]);
      if (trunc_mmin)
      {
        bi_free(mmin_[1]);
      }
    }
    if (trunc_nmax)
    {
      bi_free(nmax_[1]);
      if (trunc_nmin)
      {
        bi_free(nmin_[1]);
      }
    }
  }
}

// b > 2^e
int ge_exponent(BigInt b, int e)
{
  return bi_length(b) >= e + 1;
}

/* min <= max <= 2^(e+1) */
void split_(BigInt min, BigInt max, BigInt two_e, int e, BigInt min_[2], BigInt max_[2], int *m2,
            int *trunc_min, int *trunc_max)
{
  if (ge_exponent(min, e))
  {
    min_[0] = max_[0] = bi_zero;
    min_[1] = truncate(min, e);
    max_[1] = truncate(max, e);

    *trunc_min = 1;
    *trunc_max = 1;
  }
  else if (ge_exponent(max, e))
  {
    min_[0] = min;
    max_[0] = two_e;
    min_[1] = bi_zero;
    max_[1] = truncate(max, e);

    if (bi_iszero(min_[0]))
      *m2 += 1 << e;
    else if (e < 31)
      *m2 += (1 << e) - min_[0].digits[0];
    else
      *m2 += (1 << 31) - min_[0].digits[0];

    *trunc_min = 0;
    *trunc_max = 1;
  }
  else
  {
    min_[0] = min;
    max_[0] = max;
    min_[1] = max_[1] = bi_zero;

    *trunc_min = 0;
    *trunc_max = 0;
  }
}

BigInt truncate(BigInt b, int e)
{
  BigInt c = bi_copy(b);

  bi_flip(c, e);
  bi_flip(c, e+1);

  c.len--;
  bi_canonize(&c);

  return c;
}
