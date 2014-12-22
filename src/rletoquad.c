#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bigint.h"
#include "conversion.h"
#include "conversion_aux.h"
#include "darray.h"
#include "hashtbl.h"
#include "prgrph.h"
#include "runlength.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

/*** Matrix to- conversion ***/

Quad *rle_to_quad(Hashtbl *htbl, RleMap *rle_m)
{
  return condense(htbl, RleMap_to_QRleMap(rle_m));
}

Quad *condense(Hashtbl *htbl, struct QRleMap qrle_m)
{
}

static const struct RleLine empty_line = {
  .tokens = NULL,
  .nb_tokens = 0,
  .line_num = -1 // Does not matter
};

struct QRleMap RleMap_to_QRleMap(RleMap *rle_m)
{
  Darray *lines = da_new(sizeof(struct QRleLine));
  for ( int i = 0 ; i < rle_m->nb_lines ; )
  {
    const int i_ = i;
    struct QRleLine ql;
    struct RleLine line[2];
    if ( 1 == rle_m->lines[i].line_num % 2 ) // preceding line is empty
    {
      line[0] = empty_line;
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
      line[1] = empty_line;
      i++;
    }
    ql = fuse_RleLines(line);
    ql.line_num = rle_m->lines[i_].line_num / 2;
    da_push(lines, &ql);
  }
  struct QRleMap qrle_m;
  qrle_m.lines = da_unpack(lines, &qrle_m.nb_lines);
  return qrle_m;
}

/*! Return the fused line on success (newly allocated `.tokens`),
  a line with its `.nb_token` field set to `-1` on failure
  (unrecognized token was found).

  Leaves `.line_num` undefined! */
struct QRleLine fuse_RleLines(struct RleLine line[2])
{
  struct PopTwoTokens p2t[2];
  // Initialize p2t
  for ( int k = 0 ; k < 2 ; k++ )
    p2t[k] = p2t_new(line[k]);
  // Create line
  Darray *qtokens = da_new(sizeof(struct QRleToken));
  while ( !p2t[0].empty || !p2t[1].empty )
  {
    for ( int k = 0 ; k < 2 ; k++ )
    {
      if ( p2t[k].repeat == 0 && pop_two_tokens(&p2t[k]) )
      {
        free(da_unpack(qtokens, NULL));
        return error_QL;
      }
    }
    struct QRleToken qt = {
      .value = leaf((p2t[0].two_t << 2) | p2t[1].two_t),
      .repeat = MIN(p2t[0].repeat, p2t[1].repeat)
    };
    for ( int k = 0 ; k < 2 ; k++ )
      p2t[k].repeat -= qt.repeat;
    da_push(qtokens, &qt);
  }
  struct QRleLine ql;
  ql.tokens = da_unpack(qtokens, &ql.nb_tokens);
  return ql;
}

struct PopTwoTokens p2t_new(struct RleLine line)
{
  return (struct PopTwoTokens) {
    .pt = (struct PopToken) {
      .tokens = line.tokens,
      .nb_tokens = line.nb_tokens,
      .i = 0,
      .cur_tok.repeat = 0,
    },
    .repeat = 0,
    /* One (safe) risk of this initialization is that two empty lines
      will be popped at least once in `fuse_RleLines()` (for which the
      present function was defined) when it could return an empty line.
      However `fuse_RleLines()` is always called with at least one
      nonempty line. */
    .empty = false
  };
}

/*! Pops a token from the given array and places it in the `.cur_tok`
  field.  The current token (if any) is discarded even if its `.repeat`
  field is not 0.
  
  Return 0 on success, 1 on failure (unrecognized token).

  \see struct PopToken */
int pop_token(struct PopToken *pt)
{
  if ( pt->i == pt->nb_tokens )
  {
    // No more tokens = implicitly, infinitely many zero tokens.
    pt->cur_tok.value = 0;
    pt->cur_tok.repeat = INT_MAX; /*prepty much infinity*/
  }
  else
  {
    pt->cur_tok.repeat = pt->tokens[pt->i].repeat;
    switch ( pt->tokens[pt->i].value )
    {
      case ALIVE_CELL_CHAR:
        pt->cur_tok.value = 1;
        break;
      case DEAD_CELL_CHAR:
        pt->cur_tok.value = 0;
      default:
        perror("pop_token(): unrecognized token");
        return 1;
    }
    pt->i++;
  }
  assert( pt->cur_tok.repeat > 0 );
  return 0;
}

/*! Return 0 on success, 1 on failure
  (`pop_token()` met an unrecognized token).
  
  \see struct PopTwoTokens */
int pop_two_tokens(struct PopTwoTokens *p2t)
{
#define POP() if ( pop_token(&p2t->pt) ) return 1
  if ( p2t->pt.cur_tok.repeat == 0 )
  {
    POP();
  }
  if ( p2t->pt.cur_tok.repeat == 1 )
  {
    p2t->repeat = 1;
    p2t->two_t = p2t->pt.cur_tok.value << 1;
    POP();
    p2t->pt.cur_tok.repeat--;
    p2t->two_t |= p2t->pt.cur_tok.value;
  }
  else
  {
    p2t->repeat = p2t->pt.cur_tok.repeat / 2;
    p2t->pt.cur_tok.repeat %= 2;
    p2t->two_t = ( p2t->pt.cur_tok.value == 1 ) ? 3 : 0;
  }
  return 0;
#undef POP
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

