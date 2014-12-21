#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "conversion.h"
#include "bigint.h"
#include "hashtbl.h"
#include "darray.h"
#include "prgrph.h"
#include "runlength.h"
#include "conversion_aux.h"

/*** Matrix to- conversion ***/

Quad *rle_to_quad(Hashtbl *htbl, RleMap *rle_m)
{
  return condense(htbl, RleMap_to_QRleMap(rle_m));
}

Quad *condense(Hashtbl *htbl, struct QRleMap qrle_m)
{
}

struct QRleMap RleMap_to_QRleMap(RleMap *rle_m)
{
#define EMPTY(n) \
  (struct RleLine) { \
    .tokens = NULL, \
    .nb_tokens = 0, \
    .line_num = (n) \
  }
  Darray *lines = da_new(sizeof(struct QRleLine));
  for ( int i = 0 ; i < rle_m->nb_lines ; )
  {
    struct QRleLine ql;
    struct RleLine line[2];
    if ( 1 == rle_m->lines[i].line_num % 2 ) // preceding line is empty
    {
      line[0] = EMPTY(rle_m->lines[i].line_num - 1);
      line[1] = rle_m->lines[i];
      i++;
    }
    else if ( i + 1 < rle_m->nb_lines
           && rle_m->lines[i].line_num + 1 == rle_m->lines[i+1].line_num )
      // successive lines
    {
      line[0] = rle_m->lines[i];
      line[1] = rle_m->lines[i+1];
      i += 2;
    }
    else // following line is empty
    {
      line[0] = rle_m->lines[i];
      line[1] = EMPTY(rle_m->lines[i].line_num + 1);
      i++;
    }
    ql = fuse_RleLines(line);
    da_push(lines, &ql);
  }
  struct QRleMap qrle_m;
  qrle_m.lines = da_unpack(lines, &qrle_m.nb_lines);
  return qrle_m;
}

struct TokenTaker {
  struct RleToken *tokens;
  int nb_tokens;
  int i;
  struct RleToken cur_tok;
};

struct QRleLine fuse_RleLines(struct RleLine line[2])
{
  Darray *qtokens = da_new(sizeof(struct QRleToken));
  struct TokenTaker tt[2];
  for ( int k = 0 ; k < 2 ; k++ )
  {
    tt[k].tokens = line[k].tokens;
    tt[k].nb_tokens = line[k].nb_tokens;
    tt[k].i = 0;
    tt[k].cur_tok.repeat = 0;
  }
  while ( tt[0].i < tt[0].nb_tokens || tt[1].i < tt[1].nb_tokens )
  {
    int repeat = INT_MAX; /*pretty much infinity*/
    int leaf_id = 0;
    /* _Popping_ tokens places them in the fields of tt[]
      to make it simple to _take_ them.
      Because we are trying to zip two lines,
      we may not _take_ all of the lastly popped tokens at once.
    */
    for ( int k = 0 ; k < 2 ; k++ )
    {
      if ( tt[k].cur_tok.repeat == 0 )
      {
#define POP(k) \
        if ( tt[k].i == tt[k].nb_tokens ) \
        { \
          tt[k].cur_tok.value = 0; \
          tt[k].cur_tok.repeat = INT_MAX; /*pretty much infinity*/\
        } \
        else \
        { \
          tt[k].cur_tok.repeat = tt[k].tokens[tt[k].i].repeat; \
          switch ( tt[k].tokens[tt[k].i].value ) \
          { \
            case ALIVE_CELL_CHAR: \
              tt[k].cur_tok.value = 1; \
              break; \
            case DEAD_CELL_CHAR: \
              tt[k].cur_tok.value = 0; \
            default: \
              perror("fuse_RleLines()"); \
              exit(3); \
          }  \
          tt[k].i++; \
        }
        POP(k);
      }
      if ( tt[k].cur_tok.repeat == 1 )
      {
        repeat = 1;
        leaf_id |= tt[k].cur_tok.value << (3 - 2*k);
        POP(k); // One token taken
        leaf_id |= tt[k].cur_tok.value << (2 - 2*k);
      }
      else
      {
        int can_repeat = tt[k].cur_tok.repeat / 2;
        repeat = ( repeat < can_repeat ) ? repeat : can_repeat;
        leaf_id |= (tt[k].cur_tok.value * 3) << (2 - 2*k);
      }
    }
    for ( int k = 0 ; k < 2 ; k++ ) // Take popped token
    {
      tt[k].cur_tok.repeat -= 2 * repeat;
    }
    struct QRleToken qt = {
      .value = leaf(leaf_id),
      .repeat = repeat
    };
    da_push(qtokens, &qt);
  }
  struct QRleLine ql = { .line_num = line[0].line_num / 2 };
  ql.tokens = da_unpack(qtokens, &ql.nb_tokens);
  return ql;
}

#if 0
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
#endif


