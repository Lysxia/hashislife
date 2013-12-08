#include "hashtbl.h"
#include "hashlife.h"
#include "hbitmaps.h"
#include <stdio.h>

Quad *map_to_quad_(Hashtbl *htbl, int **map, int mlen, int nlen,
                   int mmin, int nmin, int d, int s);

Quad *map_to_quad(Hashtbl *htbl, int **map, int mlen, int nlen)
{
  int side = 2, d = 0;

  while (side < mlen || side < nlen)
  {
    side <<= 1;
    d++;
  }

  return map_to_quad_(htbl, map,mlen,nlen, 0, 0, d, side);
}

Quad *map_to_quad_(Hashtbl *htbl, int **map, int mlen, int nlen,
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
          acc |= !!map[mmin+i][nmin+j] << (i + 2 * j);
      }

    return leaf(acc);
  }
  else
  {
    Quad *quad[4];

    int i;

    s /= 2;

    for (i = 0 ; i < 4 ; i++)
      quad[i] = map_to_quad_(htbl, map,mlen,nlen,
          mmin + (i & 2 ? s : 0),
          nmin + (i & 1 ? s : 0),
          d-1, s);

    return cons_quad(htbl, quad, d);
  }
}

void quad_to_map_(int **map, int m, int n, int mlen, int nlen,
    BigInt mmin, BigInt nmin, Quad *q, int s);

void quad_to_map(int **map, int m, int n, int mlen, int nlen, BigInt mmin, BigInt nmin, Quad *q)
{
  int s = 2, d = 0;

  while (d < q->depth)
  {
    d++;
    s *= 2;
  }

  quad_to_map_(map, m, n,
    mmin, nmin, mlen, nlen,
    q, s);
}

int count = 0;

void quad_to_map_(int **map, int m, int n,
                  int mmin, int nmin,
                  int mlen, int nlen,
                  Quad *q, int s)
{
  if (mlen <= 0 || nlen <= 0)
    return;

  if (s == 2)
  {
    int i, j;
    for (i = 0 ; i < mlen ; i++)
      for (j = 0 ; j < mlen ; j++)
        map[m+i][n+j] = q->node.l.map[2*(mmin+i)+(nmin+j)];
  }
  else
  {
    s /= 2;

    int m_[2] = {m, m+s}, mmin_[2] = {mmin, 0}, mlen_[2],
        n_[2] = {n, n+s}, nmin_[2] = {nmin, 0}, nlen_[2];

    if (mmin >= s)
    {
      mmin_[1] = mmin - s;

      mlen_[0] = 0;
      mlen_[1] = mlen;
    }
    else if (mmin + mlen < s)
    {
      mlen_[0] = mlen;
      mlen_[1] = 0;
    }
    else
    {
      mlen_[0] = s - mmin;
      mlen_[1] = mlen - mlen_[0];
    }

    if (nmin >= s)
    {
      nmin_[1] = nmin - s;

      nlen_[0] = 0;
      nlen_[1] = nlen;
    }
    else if (nmin + nlen < s)
    {
      nlen_[0] = nlen;
      nlen_[1] = 0;
    }
    else
    {
      nlen_[0] = s - nmin;
      nlen_[1] = nlen - nlen_[0];
    }

    int i;

    for (i = 0 ; i < 4 ; i++)
    {
      const int x = i >> 1, y = i & 1;

      quad_to_map_(map, m_[x], n_[y],
        mmin_[x], nmin_[y],
        mlen_[x], nlen_[y],
        q->node.n.sub[i], s);
    }
  }
}
