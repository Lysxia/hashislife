#ifndef CONVERSION_AUX_H
#define CONVERSION_AUX_H

#include "bitmaps.h"
#include "hashtbl.h"
#include "prgrph.h"
#include "quad.h"
#include "runlength.h"

/*! \defgroup conversion_aux Auxiliary definitions (RLE to quadtree) */
/*!@{*/

//! Facility for acquiring RLE tokens.
/*! State for `pop_token()`.

  RLE tokens are translated into binary values `0`, `1` representing
  respectively `DEAD_CELL_CHAR`, `ALIVE_CELL_CHAR`.

  When no more tokens are available, infinitely many `0` tokens will
  be returned.
  
  The implementation guarantees that the `.tokens` array is left untouched. */
struct PopToken
{
  const struct RleToken *tokens; //!< Token buffer
  int nb_tokens;
  int i; //!< Next token index
  struct RleToken cur_tok; //!< Current token
  struct RleToken def_tok; //!< Default token to use when the buffer is spent
};

//! Facility for acquiring sequences of pairs of RLE tokens.
/*! State for `pop_two_tokens()`.
 
  When no more tokens are available, infinitely many pairs of `0`
  (corresponding to `DEAD_CELL_CHAR`) will be returned;
  `.empty` is also set to `true`. */
struct PopTwoTokens
{
  struct PopToken pt;
  union Tokenizable t[2]; //!< Two tokens
  /*!< Unrecognized tokens make the program abort */
  int repeat; //!< Token repetition
  int empty; //!< Empty token array flag
};

struct TokenCurry
{
  union Tokenizable (*f)(void *, union Tokenizable[4]);
  void *args;
};

/* Error value. Recognizable by its `.nb_token` field set to -1. */
static struct RleLine RleLine_error =
{
  .tokens = NULL,
  .nb_tokens = -1,
  .line_num = 0
};

struct RleMap prgrph_to_qrle(Prgrph p);
Quad *condense(Hashtbl *htbl, struct RleMap q_rle_m);

struct RleMap fuse_adjacent_lines(
  struct RleMap *rle_m,
  struct TokenCurry);

struct RleLine fuse_RleLines(
  struct RleLine line[2],
  struct TokenCurry);

//! Create a new `struct PopTwoTokens`
struct PopTwoTokens p2t_new(struct RleLine);
void pop_token(struct PopToken *);
void pop_two_tokens(struct PopTwoTokens *);

const struct TokenCurry token_leaf;
struct TokenCurry token_cons(Hashtbl *);
/*@}*/
#endif
