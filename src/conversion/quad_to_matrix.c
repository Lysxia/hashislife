#include <assert.h>
#include <string.h>

#include "bigint.h"
#include "conversion.h"
#include "definitions.h"
#include "lifecount.h"

/*!
  \param p Matrix (`M`-by-`N`)
  \param q Quadtree
  \param tree_h < 31. Gives zoom level (smaller = zoom out)
  \param m Vertical indices.
           The top left corner of the area to be drawn is at position
           `m.m_min` in the matrix `p`, at position `m.min` in the
           quadtree `q` (0 is the top left corner of `q`),
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
      // tree_h may be negative, in which case it is treated like
      // tree_h == 0

      {
        0 <= m.m_min < m.m_min + m.len < M
        0 <= m.min < m.min + m.len < 2^(q->depth+1)
      } || { m.len <= 0 } || { n.len <= 0 }
*/
void simple_quad_to_matrix(
  UMatrix p,
  Quad *q,
  const int tree_h, // < 31
  const struct FramePosition m,
  const struct FramePosition n)
{
  if ( m.len <= 0 || n.len <= 0 )
    return;
  else if ( tree_h <= 0 )
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
    const struct FramePosition z[2] = {m, n};
    struct FramePosition u[2][2];
    // *_[1] does not matter if *diff >= *len (*len_[1] <= 0 and stop)
    for ( int x = 0 ; x < 2 ; x++ )
    {
      const int diff = two_h - z[x].min;
      u[x][0] = (struct FramePosition) {
        // ~ (two_h < z[x].min + z[x].len ?) but protected against overflow
        // for nonnegative parameters
        .len = diff < z[x].len ? diff : z[x].len,
        .min = z[x].min,
        .m_min = z[x].m_min };
      u[x][1] = (struct FramePosition) {
        .len = z[x].min < two_h ? z[x].len - diff : z[x].len,
        .min = z[x].min < two_h ? 0 : -diff,
        .m_min = z[x].min < two_h ? z[x].m_min + diff : z[x].m_min };
    }
    for ( int i = 0 ; i < 2 ; i++ )
      for ( int j = 0 ; j < 2 ; j++ )
        simple_quad_to_matrix(
          p, q->node.n.sub[2 * i + j], tree_h-1, u[0][i], u[1][j]);
  }
}

void cropping_quad_to_matrix(
  UMatrix p,
  Quad *q,
  const int tree_h, // < 31
  struct FramePosition m,
  struct FramePosition n)
{
  if ( m.min < 0 )
  {
    m.len += m.min;
    m.min = 0;
  }
  if ( (1 << tree_h) - m.min < m.len )
  {
    m.len = (1 << tree_h) - m.min;
  }
  if ( n.min < 0 )
  {
    n.len += n.min;
    n.min = 0;
  }
  if ( (1 << tree_h) - n.min < n.len )
  {
    n.len = (1 << tree_h) - n.min;
  }
  simple_quad_to_matrix(p, q, tree_h, m, n);
}

#if 0
// This is a neat use of macros but too bad
// I finally don't want to fill out of bounds cells with zeros.
/*! Takes care of bounds outside the quadtree `q`. */
void cropping_quad_to_matrix(
  UMatrix p,
  Quad *q,
  const int tree_h, // < 31
  struct FramePosition m,
  struct FramePosition n)
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
#endif

UMatrix quad_to_matrix(
  Quad *q,
  int zoom, //! >= 0
  BigInt *mmin,
  int mlen,
  BigInt *nmin,
  int nlen)
{
  UMatrix p;

  assert( zoom >= 0 );
  if ( zoom == 0 )
  {
    p.um_char = alloc_prgrph(mlen, nlen, sizeof(char));
    if ( !p.um_char )
    {
      perror("quad_to_matrix(): char");
      return p;
    }
  }
  else
  {
    p.um_bi = alloc_prgrph(mlen, nlen, sizeof(const BigInt *));
    if ( !p.um_bi )
    {
      perror("quad_to_matrix(): bi");
      return p;
    }
  }

  const int tree_h = q->depth - zoom + 1;
  if ( tree_h <= 30 ) {
    struct FramePosition m = {
      .m_min = 0,
      .min = bi_to_int(mmin),
      .len = mlen
    };
    struct FramePosition n = {
      .m_min = 0,
      .min = bi_to_int(nmin),
      .len = nlen
    };
    simple_quad_to_matrix(p, q, tree_h, m, n);
  } else {
    struct FramePositionBig m = {
      .m_min = 0,
      .min = mmin,
      .len = mlen
    };
    struct FramePositionBig n = {
      .m_min = 0,
      .min = nmin,
      .len = nlen
    };
    quad_to_matrix_(p, q, q->depth - zoom + 1, m, n);
  }

  return p;
}

/*!
  Here we exploit the fact that the matrix size is only on 30-ish bits
  (bigger than that and we run into memory problems) to "zoom" in the
  relevant area.
*/
void quad_to_matrix_(
  UMatrix p,
  Quad *q,
  int tree_h,
  struct FramePositionBig m,
  struct FramePositionBig n)
{
  Quad *quad[4], *quad_next[4];
  memcpy(quad, q->node.n.sub, sizeof(Quad*[4]));
  for ( ; tree_h > 30 ; tree_h-- )
  {
    int y = bi_slice(m.min, tree_h-2);
    int x = bi_slice(n.min, tree_h-2);
    for ( int i = 0 ; i < 2 ; i++ )
      for ( int j = 0 ; j < 2 ; j++ )
      {
        quad_next[2*i+j] = quad[(y & 2) + ((x & 2) >> 1)]->node.n.sub[((y & 1) << 1) + (x & 1)];
      }
    memcpy(quad, quad_next, sizeof(Quad*[4]));
  }
  int mmin = bi_to_int(m.min);
  int nmin = bi_to_int(n.min);
  for ( int i = 0 ; i < 2 ; i++ )
    for ( int j = 0 ; j < 2 ; j++ )
    {
      struct FramePosition m_ = {
        .m_min = m.m_min,
        .min = mmin - (i << tree_h),
        .len = m.len
      };
      struct FramePosition n_ = {
        .m_min = n.m_min,
        .min = nmin - (j << tree_h),
        .len = n.len
      };
      cropping_quad_to_matrix(p, quad[2*i+j], tree_h, m_, n_);
    }
}

