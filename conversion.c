#include <stdio.h>
#include <stdlib.h>
#include "bigint.h"
#include "hashtbl.h"
#include "hashlife.h"
#include "conversion.h"

Quad *matrix_to_quad_(Hashtbl *htbl,
                      char **matrix, int m, int n,
                      int mmin, int nmin, int d);

Quad *matrix_to_quad(Hashtbl *htbl, const Matrix *matrix)
{
  int side = 2, d = 0;

  while ( side < matrix->m || side < matrix->n )
  {
    side <<= 1;
    d++;
  }

  return matrix_to_quad_(htbl,
                         matrix->matrix,matrix->m,matrix->n,
                         0, 0, d);
}

Quad *matrix_to_quad_(Hashtbl *htbl,
                      char **matrix, int m, int n,
                      int mmin, int nmin, int d)
{
  if ( mmin >= m || nmin >= n )
  {
    return dead_space(htbl, d);
  }
  else if ( d == 0 )
  {
    int i, j, acc = 0;
    for ( i = 0 ; i < 2 && mmin + i < m ; i++ )
      for ( j = 0 ; j < 2 && nmin + j < n ; j++ )
      {
          switch ( matrix[mmin+i][nmin+j] )
          {
            case ALIVE:
              acc |= 1 << (3 - 2 * i - j);
              break;
            case DEAD:
              break;
            default:
              fprintf(stderr, "Bad format detected\n");
        }
      }

    return leaf(acc);
  }
  else
  {
    Quad *quad[4];

    int i;
    int s = 1 << d;

    for (i = 0 ; i < 4 ; i++)
      quad[i] = matrix_to_quad_(htbl,
                                matrix,m,n,
                                mmin + (i & 2 ? s : 0),
                                nmin + (i & 1 ? s : 0),
                                d-1);

    return cons_quad(htbl, quad, d);
  }
}

void quad_to_matrix_(Matrix *matrix,
                     int m_mmin, int m_nmin,
                     BigInt *mmin, BigInt *nmin,
                     int mlen, int nlen,
                     Quad *q);

Matrix *quad_to_matrix(BigInt *mmin, BigInt *nmin,
                       int mlen, int nlen,
                       Quad *q)
{
  Matrix *matrix = malloc(sizeof(Matrix));

  if ( !matrix )
  {
    perror("quad_to_matrix()");
    return NULL;
  }

  matrix->matrix = malloc(mlen * sizeof(char *));

  if ( !matrix->matrix )
  {
    perror("quad_to_matrix()");
    free(matrix);
    return NULL;
  }
  
  int i;

  for ( i = 0 ; i < mlen ; i++ )
  {
    matrix->matrix[i] = malloc(nlen * sizeof(char));
    
    if ( !matrix->matrix[i] )
    {
      perror("quad_to_matrix()");
      matrix->m = i;
      free_matrix(matrix);
      return NULL;
    }
  }

  matrix->m = mlen;
  matrix->n = nlen;

  quad_to_matrix_(matrix,
    0, 0,
    mmin, nmin,
    mlen, nlen,
    q);

  return matrix;
}

void quad_to_matrix_(Matrix *matrix,
                     int m_mmin, int m_nmin,
                     BigInt *mmin, BigInt *nmin,
                     int mlen, int nlen,
                     Quad *q)
{
  printf("00 %d %d %d %d\n", m_mmin, m_nmin, mlen, nlen);
  if ( mlen <= 0 || nlen <= 0 )
    return;
  else if ( q->depth == 0 )
  {
    int i, j;
    int mmin_ = bi_to_int(mmin),
        nmin_ = bi_to_int(nmin);

    for ( i = 0 ; i < mlen ; i++ )
      for ( j = 0 ; j < nlen ; j++ )
        matrix->matrix[m_mmin+i][m_nmin+j] = q->node.l.map[2*(mmin_+i)+(nmin_+j)] ? ALIVE : DEAD;
  }
  else
  {
    int m_mmin_[2], m_nmin_[2];
    BigInt *mmin_[2], *nmin_[2];
    int mlen_[2], nlen_[2];

    int diff = 0;

    mmin_[0] = mmin;
    mmin_[1] = bi_minus_pow(mmin, q->depth, &diff);
    m_mmin_[0] = m_mmin;
    m_mmin_[1] = m_mmin + diff;
    mlen_[0] = mlen < diff ? mlen : diff;
    mlen_[1] = mlen - diff;

    nmin_[0] = nmin;
    nmin_[1] = bi_minus_pow(nmin, q->depth, &diff);
    m_nmin_[0] = m_nmin;
    m_nmin_[1] = m_nmin + diff;
    nlen_[0] = nlen < diff ? nlen : diff;
    nlen_[1] = nlen - diff;

    int i;

    for (i = 0 ; i < 4 ; i++)
    {
      const int x = i >> 1, y = i & 1;

      quad_to_matrix_(matrix,
                      m_mmin_[x], m_nmin_[y],
                      mmin_[x], nmin_[y],
                      mlen_[x], nlen_[y],
                      q->node.n.sub[i]);
    }

    bi_free(mmin_[1]);
    bi_free(nmin_[1]);
  }
}
