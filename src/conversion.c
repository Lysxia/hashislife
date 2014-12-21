#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "conversion.h"
#include "bigint.h"
#include "hashtbl.h"
#include "hashlife.h"
#include "lifecount.h"
#include "darray.h"
#include "prgrph.h"
#include "runlength.h"

/*** Matrix to- conversion ***/
struct Quad_repeat
{
  Quad *qr_q;
  int   qr_n;
};

struct Quad_rle_line
{
  struct Quad_repeat *qrle_line;
  int                 qrle_linelen;
  int                 qrle_linenum;
};

struct Quad_rle
{
  struct Quad_rle_line *qrle;
  int                   qrle_len;
};

const struct Quad_rle qrle_error = {
  .qrle     = NULL,
  .qrle_len = -1,
};

struct Quad_rle prgrph_to_qrle(Prgrph p);
struct Quad_rle    rle_to_qrle(Rle *rle);

Quad *condense_(Hashtbl *htbl, struct Quad_rle qrle);

struct Quad_rle map_cons(
  Hashtbl *htbl,
  Quad *ds,
  int d,
  struct Quad_rle qrle);

struct Quad_rle_line map_cons_line(
  Hashtbl *htbl,
  Quad *ds,
  int d,
  struct Quad_repeat *line1,
  int line1len,
  struct Quad_repeat *line2,
  int line2len);

int line_take_two(
  Quad *ds,
  struct Quad_repeat *line,
  int linelen,
  int *j,
  Quad *quad[]);

int rle_take_two(
  int *line,
  int linelen,
  int *j,
  int *leaf,
  int *buff);

Quad *prgrph_to_quad(Hashtbl *htbl, Prgrph p)
{
  return condense_(htbl, prgrph_to_qrle(p));
}

Quad *rle_to_quad(Hashtbl *htbl, Rle *rle)
{
  return condense_(htbl, rle_to_qrle(rle));
}

struct Quad_rle prgrph_to_qrle(Prgrph p)
{
  const int len = (p.m + 1) / 2;
  
  if ( !len )
  {
    struct Quad_rle zero = {
      .qrle = NULL,
      .qrle_len = -1,
    };
    return zero;
  }

  struct Quad_rle_line *lines =
    malloc(len * sizeof(struct Quad_rle_line));

  if ( !lines )
  {
    perror("prgrph_to_qrle()");
    return qrle_error;
  }

  int i, j;
  for ( i = 0 ; i < len ; i++ )
  {
    int len0 = strlen(p.prgrph[2*i]);
    int len1 = 2 * i + 1 < p.m ? strlen(p.prgrph[2*i+1]) : 0;
    lines[i].qrle_linenum = i;
    lines[i].qrle_linelen = ((len0 < len1 ? len1 : len0) + 1) / 2;
    lines[i].qrle_line =
      malloc(lines[i].qrle_linelen * sizeof(struct Quad_repeat));

    if ( !lines[i].qrle_line )
    {
      for ( i-- ; i >= 0 ; i-- )
        free(lines[i].qrle_line);
      free(lines);
      return qrle_error;
    }

    for ( j = 0 ; 2 * j < len0 || 2 * j < len1 ; j++ )
    {
      int l = 0;
      if ( 2 * j < len0 )
      {
        l |= (p.prgrph[2*i][2*j] == ALIVE) << 3;
        if ( 2 * j + 1 < len0 )
          l |= (p.prgrph[2*i][2*j+1] == ALIVE) << 2;
      }
      if ( 2 * i + 1 < p.m && 2 * j < len1 )
      {
        l |= (p.prgrph[2*i+1][2*j] == ALIVE) << 1;
        if ( 2 * j + 1 < len1 )
          l |= (p.prgrph[2*i+1][2*j+1] == ALIVE);
      }

      lines[i].qrle_line[j].qr_q = leaf(l);
      lines[i].qrle_line[j].qr_n = 1;
    }
  }

  struct Quad_rle rle = {
    .qrle = lines,
    .qrle_len = len,
  };

  return rle;
}

struct Quad_rle rle_to_qrle(Rle *rle)
{
  Darray *da = da_new(sizeof(struct Quad_rle_line));

  int i = 0;
  while ( i < rle->rle_lines_c )
  {
    int *line[2], j[2] = {0}, buff[2] = {-1, -1}, l[2], n[2] = {0}, len[2];
    struct Quad_rle_line qrle_l;
    qrle_l.qrle_linenum = rle->rle_lines[i].line_num / 2;
    Darray *da_line = da_new(sizeof(struct Quad_repeat));

    if ( 1 == rle->rle_lines[i].line_num % 2 )
    {
      line[0] = NULL;
      len[0] = 0;
      line[1] = rle->rle_lines[i].line;
      len[1] = rle->rle_lines[i].line_length;
      i++;
    }
    else
    {
      line[0] = rle->rle_lines[i].line;
      len[0] = rle->rle_lines[i].line_length;

      if ( i + 1 < rle->rle_lines_c
        && rle->rle_lines[i+1].line_num == rle->rle_lines[i].line_num + 1 )
      {
        line[1] = rle->rle_lines[i+1].line;
        len[1] = rle->rle_lines[i+1].line_length;
        i += 2;
      }
      else
      {
        line[1] = NULL;
        len[1] = 0;
        i++;
      }
    }

    while ( j[0] < len[0] || j[1] < len[1] )
    {
      int k;
      for ( k = 0 ; k < 2 ; k++ )
        if ( n[k] == 0 )
          n[k] = rle_take_two(line[k], len[k], &j[k], &l[k], &buff[k]);

      struct Quad_repeat qr;

      qr.qr_q = leaf(l[0] << 2 | l[1]);

      if ( n[0] < n[1] )
      {
        qr.qr_n = n[0];
        n[1] -= n[0];
        n[0] = 0;
      }
      else
      {
        qr.qr_n = n[1];
        n[0] -= n[1];
        n[1] = 0;
      }

      da_push(da_line, &qr);
    }

    qrle_l.qrle_line = da_unpack(da_line, &qrle_l.qrle_linelen);
    da_push(da, &qrle_l);
  }

  struct Quad_rle qrle;
  qrle.qrle = da_unpack(da, &qrle.qrle_len);

  return qrle;
}

Quad *condense_(Hashtbl *htbl, struct Quad_rle qrle)
{
  if ( qrle.qrle_len == -1 )
    return NULL;
  else if ( qrle.qrle_len == 0 )
    return dead_space(htbl, 0);

  int d;
  for ( d = 0 ; qrle.qrle_len > 1 || qrle.qrle[0].qrle_linelen > 1 ; d++ )
  {
    {
      printf("-%d-", qrle.qrle_len);
      int i;
      for ( i = 0 ; i < qrle.qrle_len ; i++ )
        printf("%d %d, ", qrle.qrle[i].qrle_linelen, qrle.qrle[i].qrle_linenum);
    }
    qrle = map_cons(htbl, dead_space(htbl, d), d+1, qrle);
    {
      printf("\n");
      fflush(stdout);
    }
  }

  Quad *q = qrle.qrle[0].qrle_line[0].qr_q;

  free(qrle.qrle[0].qrle_line);
  free(qrle.qrle);
 
  return q;
}

struct Quad_rle map_cons(
  Hashtbl *htbl,
  Quad *ds,
  int d,
  struct Quad_rle qrle)
{
  struct Quad_rle qrle2;

  qrle2.qrle = malloc(qrle.qrle_len * sizeof(struct Quad_rle_line));

  if ( !qrle2.qrle )
  {
    perror("map_cons()");
    qrle2.qrle_len = -1;
    return qrle2;
  }

  int i = 0, l = 0;
  while ( i < qrle.qrle_len )
  {
    int linenum = qrle.qrle[i].qrle_linenum / 2;
    if ( qrle.qrle[i].qrle_linenum % 2 == 1 )
    {
      qrle2.qrle[l] =
        map_cons_line(htbl, ds, d,
                      NULL, 0,
                      qrle.qrle[i].qrle_line, qrle.qrle[i].qrle_linelen);
      free(qrle.qrle[i].qrle_line);
      i++;
    }
    else if ( i + 1 < qrle.qrle_len
           && qrle.qrle[i+1].qrle_linenum == qrle.qrle[i].qrle_linenum + 1 )
    {
      qrle2.qrle[l] =
        map_cons_line(htbl, ds, d,
                      qrle.qrle[i].qrle_line, qrle.qrle[i].qrle_linelen,
                      qrle.qrle[i+1].qrle_line, qrle.qrle[i+1].qrle_linelen);
      free(qrle.qrle[i].qrle_line);
      free(qrle.qrle[i+1].qrle_line);
      i += 2;
    }
    else
    {
      qrle2.qrle[l] =
        map_cons_line(htbl, ds, d,
                      qrle.qrle[i].qrle_line, qrle.qrle[i].qrle_linelen,
                      NULL, 0);
      free(qrle.qrle[i].qrle_line);
      i++;
    }
    qrle2.qrle[l].qrle_linenum = linenum;
    l++;
  }

  free(qrle.qrle);
  qrle2.qrle_len = l;

  return qrle2;
}

struct Quad_rle_line map_cons_line(
  Hashtbl *htbl,
  Quad *ds,
  int d,
  struct Quad_repeat *line1,
  int line1len,
  struct Quad_repeat *line2,
  int line2len)
{
  int j1 = 0, j2 = 0;
  int id1 = 0, id2 = 0;
  Quad *quad[4];

  Darray *da = da_new(sizeof(struct Quad_repeat));

  while ( j1 < line1len || j2 < line2len )
  {
    if ( id1 == 0 )
      id1 = line_take_two(ds, line1, line1len, &j1, quad);
    if ( id2 == 0 )
      id2 = line_take_two(ds, line2, line2len, &j2, quad+2);

    struct Quad_repeat qr;

    qr.qr_q = cons_quad(htbl, quad, d);

    if ( id1 < id2 )
    {
      qr.qr_n = id1;
      id2 -= id1;
      id1 = 0;
    }
    else
    {
      qr.qr_n = id2;
      id1 -= id2;
      id2 = 0;
    }

    da_push(da, &qr);
  }

  struct Quad_rle_line line_;

  line_.qrle_line = da_unpack(da, &line_.qrle_linelen);

  return line_;
}

int line_take_two(
  Quad *ds,
  struct Quad_repeat *line,
  int linelen,
  int *j,
  Quad *quad[])
{
  if ( *j == linelen )
  {
    quad[0] = quad[1] = ds;
    return INT_MAX;
  }
  else if ( line[*j].qr_n == 1 )
  {
    quad[0] = line[*j].qr_q;
    ++*j;
    if ( *j == linelen )
      quad[1] = ds;
    else
    {
      quad[1] = line[*j].qr_q;
      if ( 0 == --line[*j].qr_n )
        ++*j;
    }
    return 1;
  }
  else
  {
    quad[0] = quad[1] = line[*j].qr_q;
    int len = line[*j].qr_n / 2;
    if ( 0 == (line[*j].qr_n %= 2) )
      ++*j;
    return len;
  }
}

// At the first call buff must be initialized to -1
// and not modified by the caller thereafter
int rle_take_two(
  int *line,
  int linelen,
  int *j,
  int *leaf,
  int *buff)
{
  if ( *j == linelen )
  {
    *leaf = 0;
    return INT_MAX;
  }

  if ( *buff == -1 )
    *buff = line[*j];

  int len;

  if ( *buff == 1 )
  {
    *leaf = (*j & 1) << 1;
    len = 1;

    if ( ++*j < linelen )
    {
      *leaf |= *j & 1;
      *buff = line[*j] - 1;
      len = 1;
    }
  }
  else
  {
    *leaf = (*j & 1) ? 3 : 0;
    len = *buff / 2;
    *buff %= 2;
  }

  if ( *buff == 0 && ++*j < linelen )
    *buff = line[*j];

  return len;
}

/*** -to matrix conversion ***/

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
  if ( mlen <= 0 || nlen <= 0 )
    return;
  else if ( q->depth <= height - 1 )
  {
    int i, j;
    for ( i = 0 ; i < mlen ; i++ )
      for ( j = 0 ; j < nlen ; j++ )
      {
        p.um_bi[m_mmin+i][m_nmin+j] =
          i || j ? bi_zero_const : cell_count(q);
      }
  }
  else if ( q->depth == 0 )
  {
    int i, j;
    int mmin_ = bi_to_int(mmin),
        nmin_ = bi_to_int(nmin);

    for ( i = 0 ; i < mlen ; i++ )
      for ( j = 0 ; j < nlen ; j++ )
      {
        p.um_char[m_mmin+i][m_nmin+j] =
          (mmin_+i < 2 && nmin_+j < 2 && q->node.l.map[2*(mmin_+i)+(nmin_+j)])
          ? ALIVE : DEAD;
      }
  }
  else
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
}

Prgrph bi_mat_to_prgrph(const BigInt ***bm, int m, int n, int height)
{
  Prgrph p;

  p.prgrph = alloc_prgrph(m, n, sizeof(char));
  p.m = m;

  int i, j;
  for ( i = 0 ; i < m ; i++ )
    for ( j = 0 ; j < n ; j++ )
    {
      const int hex_max = 16;
      int u = bi_log2(bm[i][j]);
      int x = u / ((2 * height) / hex_max + 1);
      p.prgrph[i][j] = u               // u == 0 <=> bm[i][j] is zero
                             ? x > 9
                               ? 'A' - 10 + x
                               : '0' + x
                             : '.';
    }

  return p;
}

void free_um_char(UMatrix um, int m)
{
  int i;
  for ( i = 0 ; i < m ; i++ )
    free(um.um_char[i]);
  free(um.um_char);
}

void free_um_bi(UMatrix um, int m)
{
  int i;
  for ( i = 0 ; i < m ; i++ )
    free(um.um_bi[i]);
  free(um.um_char);
}
