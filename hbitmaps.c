#include "hashtbl.h"
#include "hashlife.h"
#include "hbitmaps.h"

Quad* map_to_quad_(int** map, int m, int n,
                   int mmin, int mmax, int nmin, int nmax, int d);

Quad* map_to_quad(int** map, int m, int n)
{
  int side = 2, d = 0;

  while (side < m || side < n)
  {
    side <<= 1;
    d++;
  }

  return map_to_quad_(map,m,n, 0,side, 0,side, d);
}

Quad* map_to_quad_(int** map, int m, int n,
                int mmin, int mmax,
                int nmin, int nmax, int d)
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
          acc += map[mmin+i][nmin+j] != 0; // 0 if cell is 0, 1 otherwise
      }
    return leaves + acc;
  }
  else
  {
    Quad *quad[4];
    int mmid = (mmin + mmax) / 2, nmid = (nmin + nmax) / 2;

    quad[0] = map_to_quad_(map,m,n, mmin,mmid, nmin,nmid, d-1);
    quad[1] = map_to_quad_(map,m,n, mmin,mmid, nmid,nmax, d-1);
    quad[2] = map_to_quad_(map,m,n, mmid,mmax, nmin,nmid, d-1);
    quad[3] = map_to_quad_(map,m,n, mmid,mmax, nmid,nmax, d-1);

    return cons_quad(quad,d);
  }
}

void quad_to_map_(int** map, int m, int n, int mmin, int mmax, int nmin, int nmax, Quad* q, int s);
int clip(int a, int b, int c);

void quad_to_map(int** map, int mmin, int mmax, int nmin, int nmax, Quad* q)
{
  int s = 2, d = 0;

  while (d < q->depth)
  {
    d++;
    s *= 2;
  }

  quad_to_map_(map, 0, 0,
      clip(mmin, 0, s), clip(mmax, 0, s),
      clip(nmin, 0, s), clip(nmax, 0, s),
      q, s);
}

int clip(int a, int b, int c)
{
  if (a < b)
    return b;
  else if (a > c)
    return c;
  else
    return a;
}

void quad_to_map_(int** map, int m, int n,
    int mmin, int mmax, int nmin, int nmax,
    Quad* q, int s)
{
  if (mmax == 0 || nmax == 0 || mmin == s || nmin == s || mmin >= mmax || nmin >= nmax)
  {
    int i, j;
    for (i = 0 ; i < mmax - mmin ; i++)
      for (j = 0 ; j < nmax - nmin ; j++)
        map[i][j] = 0;
  }
  else if (s == 2)
  {
    int i, j;
    for (i = mmin ; i < mmax ; i++)
      for (j = nmin ; j < nmax ; j++)
        map[i][j] = q->node.l.map[2*i+j];
  }
  else
  {
    s /= 2;

    int i;

    for (i = 0 ; i < 4 ; i++)
    {
      int mshift = (i & 1) ? s : 0, nshift = (i & 2) ? s : 0;
      quad_to_map_(map,m + mshift, n + nshift,
        clip(mmin - mshift, 0, s), clip(mmax - mshift, 0, s),
        clip(nmin - nshift, 0, s), clip(nmax - nshift, 0, s),
        q, s);
    }
  }
}
