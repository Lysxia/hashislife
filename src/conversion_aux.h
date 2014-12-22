#ifndef CONVERSION_AUX_H
#define CONVERSION_AUX_H

#include "bitmaps.h"
#include "hashtbl.h"
#include "prgrph.h"
#include "quad.h"
#include "runlength.h"

/*! \defgroup conversion_aux Auxiliary definitions (RLE to quadtree) */
/*!@{*/

/* Mirroring the basic RLE structures */ 
struct QRleToken
{
  Quad *value;
  int   repeat;
};

struct QRleLine
{
  struct QRleToken *tokens;
  int nb_tokens;
  int line_num;
};

struct QRleMap
{
  struct QRleLine *lines;
  int nb_lines;
};

//! Facility for acquiring RLE tokens.
/*! State for `pop_token()`.

  RLE tokens are translated into binary values `0`, `1` representing
  respectively `DEAD_CELL_CHAR`, `ALIVE_CELL_CHAR`.

  When no more tokens are available, infinitely many `0` tokens will
  be returned.
  
  The implementation guarantees that the `.tokens` array is lefti untouched. */
struct PopToken
{
  const struct RleToken *tokens; //!< Token buffer
  int nb_tokens;
  int i; //!< Next token index
  struct RleToken cur_tok; //!< Current token
};

//! Facility for acquiring sequences of pairs of RLE tokens.
/*! State for `pop_two_tokens()`.
 
  When no more tokens are available, infinitely many pairs of `0`
  (corresponding to `DEAD_CELL_CHAR`) will be returned;
  `.empty` is also set to `true`. */
struct PopTwoTokens
{
  struct PopToken pt;
  unsigned two_t; //!< Two tokens are read into the two least significant bits.
               /*!< Unrecognized tokens make the program abort */
  int repeat;
  int empty;
};

/* Error value. Recognizable by its `.nb_token` field set to -1. */
static struct QRleLine error_QL =
{
  .tokens = NULL,
  .nb_tokens = -1,
  .line_num = 0
};

struct QRleMap prgrph_to_qrle(Prgrph p);
struct QRleMap RleMap_to_QRleMap(RleMap *rle_m);

Quad *condense(Hashtbl *htbl, struct QRleMap qrle_m);

struct QRleLine fuse_RleLines(struct RleLine line[2]);

//! Create a new `struct PopTwoTokens`
struct PopTwoTokens p2t_new(struct RleLine);
int pop_token(struct PopToken *);
int pop_two_tokens(struct PopTwoTokens *);

void QRleMap_delete(struct QRleMap);
/*@}*/
#endif
