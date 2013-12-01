#include "hashtbl.h"
#include "hashlife.h"
#include "hbitmaps.h"
#include <stdio.h>

Quad *map_to_quad_(int **map, int m, int n,
                   int mmin, int nmin, int d, int s);

Quad *map_to_quad(int **map, int m, int n)
{
  int side = 2, d = 0;

  while (side < m || side < n)
  {
    side <<= 1;
    d++;
  }

  return map_to_quad_(map,m,n, 0, 0, d, side);
}

Quad *map_to_quad_(int **map, int m, int n,
                   int mmin, int nmin, int d, int s)
{
  if (mmin >= m || nmin >= n)
  {
    return dead_space(d);
  }
  else if (d == 0)
  {
    int i, j, acc = 0;
    for (i = 0 ; i < 2 ; i++)
      for (j = 0 ; j < 2 ; j++)
      {
        acc <<= 1;
        if (mmin + i >= 0 && mmin + i < m && nmin + j >= 0 && nmin + j < n)
          acc |= !!map[mmin+i][nmin+j];
        putchar('0' + (acc & 1));
      }
    putchar('\n');

    return leaves + acc;
  }
  else
  {
    Quad *quad[4];

    int i;

    s /= 2;

    for (i = 0 ; i < 4 ; i++)
      quad[i] = map_to_quad_(map,m,n,
          mmin + (i & 2 ? s : 0),
          nmin + (i & 1 ? s : 0),
          d-1, s);

    return cons_quad(quad,d);
  }
}

void zero_(int **map, int m, int mlen, int n, int nlen);
void quad_to_map_(int **map, int m, int n, int mmin, int mlen, int nmin, int nlen, Quad *q, int s);

void quad_to_map(int **map, int m, int n, int mmin, int mlen, int nmin, int nlen, Quad *q)
{
  int s = 2, d = 0;

  while (d < q->depth)
  {
    d++;
    s *= 2;
  }

  if (mlen <= 0 || nlen <= 0)
    return;

  if (mmin < 0)
  {
    int mlen2 = mmin + mlen <= 0 ? mlen : -mmin;

    zero_(map, m, mlen2, n, nlen);

    m += mlen2;
    mmin = 0;
    mlen -= mlen2;
  }

  if (mmin+mlen > s)
  {
    int mmin2 = mmin < s ? s : mmin;
    int m2 = m + mmin2 - mmin,
        mlen2 = mlen - mmin2 + mmin;

    zero_(map, m2, mlen2, n, nlen);

    mlen -= mlen2;
  }

  if (nmin < 0)
  {
    int nlen2 = nmin + nlen <= 0 ? nlen : -nmin;

    zero_(map, m, mlen, n, nlen2);

    n += nlen2;
    nmin = 0;
    nlen -= nlen2;
  }

  if (nmin + nlen > s)
  {
    int nmin2 = nmin < s ? s : nmin;
    int n2 = n + nmin2 - nmin,
        nlen2 = nlen - nmin2 + nmin;

    zero_(map, m, mlen, n2, nlen2);

    nlen -= nlen2;
  }

  if (mlen == 0 || nlen == 0)
    return;
  else
    quad_to_map_(map, m, n,
      mmin, mlen, nmin, nlen,
      q, s);
}

void zero_(int **map, int m, int mlen, int n, int nlen)
{
  int i, j;
  for (i = 0 ; i < mlen ; i++)
    for (j = 0 ; j < nlen ; j++)
      map[m+i][n+j] = 0;
}

void quad_to_map_(int **map, int m, int n,
                  int mmin, int mlen, int nmin, int nlen,
                  Quad *q, int s)
{
  if (mlen <= 0 || nlen <= 0)
    return;

  if (s == 2)
  {
    int i, j;
    for (i = mmin ; i < mmin + mlen ; i++)
      for (j = nmin ; j < nmin + nlen ; j++)
        map[m+i][n+j] = q->node.l.map[2*i+j];
  }
  else
  {
    s /= 2;

    int mlen2 = 0, nlen2 = 0;

    if (mlen > s)
    {
      mlen2 = mlen - s;
      mlen = s;
    }

    if (nlen > s)
    {
      nlen2 = nlen - s;
      nlen = s;
    }

    int i;

    for (i = 0 ; i < 4 ; i++)
    {
      int mshift = 0, mlen_ = mlen, nshift = 0, nlen_ = nlen;

      if (i & 2)
      {
        mshift = s;
        mlen_ = mlen2;
      }

      if (i & 1)
      {
        nshift = s;
        nlen_ = nlen2;
      }

      quad_to_map_(map, m + mshift, n + nshift,
        mmin - mshift, mlen_,
        nmin - nshift, nlen_,
        q, s);
    }
  }
}
