#include <assert.h>

#include "bigint.h"
#include "conversion.h"
#include "definitions.h"
#include "lifecount.h"

void quad_to_matrix_(
  UMatrix p,
  int m_mmin,
  int m_nmin,
  BigInt *mmin,
  BigInt *nmin,
  int mlen,
  int nlen,
  const int height_,
  Quad *q);

struct FrameIndices {
  int m_min;
  int min;
  int len;
};

/*!
  \param p Matrix (`M`-by-`N`)
  \param q Quadtree
  \param tree_h Gives zoom level (smaller = zoom out)
  \param m Vertical indices.
           The top left corner of the area to be drawn
           is at position `m.m_min` in the matrix `p`,
           at position `m.min` in the quadtree `q`
           (0 is the top left corner of `q`),
           and has height `m.len`.
  \param n Horizontal indices

  E.g. if `q` represents this 4-by-4 area:
  
      0 1 2 3
      4 5 6 7
      8 9 A B
      C D E F

  and we want to draw the 2-by-3 area with top left corner 9,
  inside a `.` initialized 5-by-5 matrix `p` to obtain
  (`p[1][2]` is 9):

      . . . . .
      . . 9 A B
      . . D E F
      . . . . .
      . . . . .

  we need to give the coordinates

      m = {
        .m_min = 1, // vertical position of 9 in p
        .min = 2,   // v. position of 9 in q
        .len = 2 }  // height of the area
      n = {
        .m_min = 2, // h. position of 9 in p
        .min = 1,   // h. position of 9 in q
        .len = 3 }  // width of the area

  Invariants:

      tree_h - 1 <= q->depth

      {
        0 <= m.m_min < m.m_min + m.len < M
        0 <= m.min < m.min + m.len < 2^(q->depth+1)
      } || { m.len <= 0 } || { n.len <= 0 }
*/
void simple_quad_to_matrix(
  UMatrix p,
  Quad *q,
  const int tree_h, // < 31
  const struct FrameIndices m,
  const struct FrameIndices n)
{
  if ( m.len <= 0 || n.len <= 0 )
    return;
  else if ( tree_h == 0 )
  {
    assert( m.len == 1 && n.len == 1 );
    p.um_bi[m.m_min][n.m_min] = cell_count(q);
  }
  else if ( q->depth == 0 && tree_h == 1)
  {
    assert( m.min+m.len <= 2 && n.min+n.len <= 2 );
    for ( int i = 0 ; i < m.len ; i++ )
      for ( int j = 0 ; j < n.len ; j++ )
      {
        p.um_char[m.m_min+i][n.m_min+j]
          = q->node.l.map[2 * (m.min+i) + (n.min+j)]
          ? ALIVE_CELL_CHAR : DEAD_CELL_CHAR;
      }
  }
  else
  {
    const int two_h = 1 << (tree_h - 1);
    const struct FrameIndices z[2] = {m, n};
    struct FrameIndices u[2][2];
    // *_[1] does not matter if *diff >= *len (*len_[1] <= 0 and stop)
    for ( int x = 0 ; x < 2 ; x++ )
    {
      int diff = two_h - z[x].min;
      u[x][0] = (struct FrameIndices) {
        .len = diff < z[x].len ? diff : z[x].len,
        .min = z[x].min,
        .m_min = z[x].m_min };
      u[x][1] = (struct FrameIndices) {
        .len = z[x].len - diff,
        .min = diff < 0 ? -diff : 0,
        .m_min = ( 0 < diff ) ? z[x].m_min + z[x].len : z[x].m_min };
    }
    for ( int i = 0 ; i < 2 ; i++ )
      for ( int j = 0 ; j < 2 ; j++ )
        simple_quad_to_matrix(
          p, q->node.n.sub[2 * i + j], tree_h-1, u[0][i], u[1][j]);
  }
}

/*! Takes care of bounds outside the quadtree `q`. */
void cropping_quad_to_matrix(
  UMatrix p,
  Quad *q,
  const int tree_h, // < 31
  struct FrameIndices m,
  struct FrameIndices n)
{
#define FILL(imin,imax,jmin,jmax) \
  { \
    for ( int i = (imin) ; i < (imax) ; i++ ) \
      for ( int j = (jmin) ; j < (jmax) ; j++ ) \
        if ( tree_h - 1 == q->depth ) \
          p.um_char[m.m_min+i][n.m_min+j] = DEAD_CELL_CHAR; \
        else \
          p.um_bi[m.m_min+i][n.m_min+j] = bi_zero_const; \
  }
  if ( m.min < 0 )
  {
    FILL(0, -m.min, 0, n.len);
    m.len += m.min;
    m.min = 0;
  }
  if ( (1 << tree_h) - m.min < m.len )
  {
    FILL((1 << tree_h) - m.min, m.len, 0, n.len);
    m.len = (1 << tree_h) - m.min;
  }
  if ( n.min < 0 )
  {
    FILL(0, m.len, 0, -n.min);
    n.len += n.min;
    n.min = 0;
  }
  if ( (1 << tree_h) - n.min < n.len )
  {
    FILL(0, m.len, (1 << tree_h) - n.min, n.len);
    n.len = (1 << tree_h) - n.min;
  }
#undef FILL
  simple_quad_to_matrix(p, q, tree_h, m, n);
}

UMatrix quad_to_matrix(
  BigInt *mmin,
  BigInt *nmin,
  int mlen,
  int nlen,
  int height,
  Quad *q)
{
  UMatrix p;

  if ( height <= 0 )
  {
    height = 0;

    p.um_char = alloc_prgrph(mlen, nlen, sizeof(char));

    if ( !p.um_char )
    {
      perror("quad_to_matrix()");
      return p;
    }
  }
  else
  {
    p.um_bi = alloc_prgrph(mlen, nlen, sizeof(const BigInt *));

    if ( !p.um_bi )
    {
      perror("quad_to_matrix()");
      return p;
    }
  }

  quad_to_matrix_(p,
    0, 0,
    mmin, nmin,
    mlen, nlen,
    height, q);

  return p;
}

void quad_to_matrix_(
  UMatrix p,
  int m_mmin,
  int m_nmin,
  BigInt *mmin,
  BigInt *nmin,
  int mlen,
  int nlen,
  const int height,
  Quad *q)
{
    int m_mmin_[2], m_nmin_[2];
    BigInt *mmin_[2], *nmin_[2];
    int mlen_[2], nlen_[2];

    int diff = 0;

    mmin_[0] = mmin;
    mmin_[1] = bi_minus_pow(mmin, q->depth - height, &diff);
    m_mmin_[0] = m_mmin;
    m_mmin_[1] = m_mmin + diff;
    mlen_[0] = mlen < diff ? mlen : diff;
    mlen_[1] = mlen - diff;

    nmin_[0] = nmin;
    nmin_[1] = bi_minus_pow(nmin, q->depth - height, &diff);
    m_nmin_[0] = m_nmin;
    m_nmin_[1] = m_nmin + diff;
    nlen_[0] = nlen < diff ? nlen : diff;
    nlen_[1] = nlen - diff;

    int i;

    for ( i = 0 ; i < 4 ; i++ )
    {
      const int x = i >> 1, y = i & 1;

      quad_to_matrix_(p,
                      m_mmin_[x], m_nmin_[y],
                      mmin_[x], nmin_[y],
                      mlen_[x], nlen_[y],
                      height, q->node.n.sub[i]);
    }

    bi_free(mmin_[1]);
    bi_free(nmin_[1]);
}

