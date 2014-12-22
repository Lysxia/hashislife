#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bigint.h"
#include "conversion.h"
#include "conversion_aux.h"
#include "hashtbl.h"
#include "darray.h"
#include "prgrph.h"
#include "runlength.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

/*** Matrix to- conversion ***/

static const struct RleLine empty_line =
{
  .tokens = NULL,
  .nb_tokens = 0,
  .line_num = -1 // Does not matter
};

union Tokenizable token_leaf_(void *ptr, union Tokenizable cells[4])
{
  (void) ptr;
  unsigned l = 0;
  for ( int i = 0 ; i < 4 ; i++ )
  {
    assert( cells[i].int_ == 0 || cells[i].int_ == 1 );
    l |= cells[i].int_ << (3 - i);
  }
  return (union Tokenizable) { .ptr_ = leaf(l) };
}

const struct TokenCurry token_leaf =
{
  .f = &token_leaf_,
  .args = NULL,
};

/* Variables corresponding to RLE encoding of `Quad*` arrays are
  prefixed with `q_`. The fusion functions have been generalized but
  the notation we use is within the context of the fusion of two
  "bit" RleMap into a "quadtree" RleMap. */
struct RleMap fuse_adjacent_lines(
  struct RleMap *rle_m,
  struct TokenCurry f) //!< Fusion function
{
  Darray *lines = da_new(sizeof(struct RleLine));
  for ( int i = 0 ; i < rle_m->nb_lines ; )
  {
    const int i_ = i;
    struct RleLine q_l;
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
    q_l = fuse_RleLines(line, f);
    q_l.line_num = rle_m->lines[i_].line_num / 2;
    da_push(lines, &q_l);
  }
  struct RleMap q_rle_m;
  q_rle_m.lines = da_unpack(lines, &q_rle_m.nb_lines);
  return q_rle_m;
}

/* \deprecated Return the fused line on success (newly allocated `.tokens`),
  a line with its `.nb_token` field set to `-1` on failure
  (unrecognized token was found). */
/*! Leaves `.line_num` undefined! */
struct RleLine fuse_RleLines(
  struct RleLine line[2], //!< Two lines to fuse
  struct TokenCurry f) //!< Fusion function
{
  struct PopTwoTokens p2t[2];
  // Initialize p2t
  for ( int k = 0 ; k < 2 ; k++ )
    p2t[k] = p2t_new(line[k]);
  // Create line
  Darray *q_tokens = da_new(sizeof(struct RleToken));
  while ( !p2t[0].empty || !p2t[1].empty )
  {
    // Pop a pair of tokens (possibly repeated) on each line
    for ( int k = 0 ; k < 2 ; k++ )
    {
      if ( p2t[k].repeat == 0 )
        pop_two_tokens(&p2t[k]);
    }
    /* Make a quad leaf (possibly repeated, at most as many
      times as the least repeated pair) */
    union Tokenizable q_ts[4];
    for ( int l = 0 ; l < 4 ; l++ )
      q_ts[l] = p2t[l >> 1].t[l & 1];
    struct RleToken q_t = {
      .value = (*f.f)(f.args, q_ts),
      .repeat = MIN(p2t[0].repeat, p2t[1].repeat)
    };
    // Consume the token pairs
    for ( int k = 0 ; k < 2 ; k++ )
      p2t[k].repeat -= q_t.repeat;
    da_push(q_tokens, &q_t);
  }
  struct RleLine q_l;
  q_l.tokens = da_unpack(q_tokens, &q_l.nb_tokens);
  return q_l;
}

/*!
  \see struct PopTwoTokens */
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
  
  \see struct PopToken */
void pop_token(struct PopToken *pt)
{
  if ( pt->i == pt->nb_tokens )
    pt->cur_tok = pt->def_tok;
  else
  {
    pt->cur_tok.repeat = pt->tokens[pt->i].repeat;
    pt->cur_tok.value = pt->tokens[pt->i].value;
    pt->i++;
  }
  assert( pt->cur_tok.repeat > 0 );
}

/*! Return 0 on success, 1 on failure
  (`pop_token()` met an unrecognized token).
  
  \see struct PopTwoTokens */
void pop_two_tokens(struct PopTwoTokens *p2t)
{
#define POP() pop_token(&p2t->pt)
  if ( p2t->pt.cur_tok.repeat == 0 )
  {
    POP();
  }
  if ( p2t->pt.cur_tok.repeat == 1 )
  {
    p2t->repeat = 1;
    p2t->t[0] = p2t->pt.cur_tok.value;
    POP();
    p2t->pt.cur_tok.repeat--;
    p2t->t[1] = p2t->pt.cur_tok.value;
  }
  else
  {
    p2t->repeat = p2t->pt.cur_tok.repeat / 2;
    p2t->pt.cur_tok.repeat %= 2;
    p2t->t[0] = p2t->t[1] = p2t->pt.cur_tok.value;
  }
#undef POP
}

