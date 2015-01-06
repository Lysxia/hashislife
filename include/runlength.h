#ifndef RUNLENGTH_H
#define RUNLENGTH_H

#include <stdio.h>
#include <stdlib.h>

#include "darray.h"
#include "definitions.h"

/*! \defgroup rle Run length encoding */
/*!@{*/
#define     END_RLE_TOKEN '!'
#define    DEAD_RLE_TOKEN 'b'
#define   ALIVE_RLE_TOKEN 'o'
#define NEWLINE_RLE_TOKEN '$'

//! Polymorphism!
/*! A union for `struct RleToken` value field.
  This is convenient because the same structure of run length encoded
  lines as in `bitmaps.h` and the same algorithm are used to
  hashcons these streams of tokens into quadtrees. */
union Tokenizable
{
  char char_;
  int int_;
  void *ptr_;
};

//! Generic repeated value
/*! Run length encoding encodes successive equal values by prefixing
  the value with the number of repetitions. A safer version which deals
  with larger ranges, as is part of the goal of this project, would
  use big ints. */ /* TODO */
struct RleToken
{
  union Tokenizable value;
  int repeat;
};

//! State used by the `life_rle_write()` implementation
/*! Carry information between calls to write_one_token. */
struct TokenWriter
{
  FILE *file;
  int   line_length;
};

//! Game of Life .rle file contents
struct LifeRle
{
  struct RleToken *tokens; //!< An array of `char` tokens
  /*!< The tokens have one of the following values:

     END_RLE_TOKEN
     DEAD_RLE_TOKEN
     ALIVE_RLE_TOKEN
     NEWLINE_RLE_TOKEN
      
    The token `END_RLE_TOKEN` indicates the end of the array. */
  int x;
  int y;
  rule r;
};

extern FILE *life_rle_in;
//! Parse a file in *run length encoded* format
/*! The argument must be a non-`NULL` pointer to `struct LifeRle`.
  It is initialized with a runlength encoding of the file contents.

  In case of failure the fields of the argument are undefined.

  Return values:
  - 0 on success;
  - 1 on other errors (see `errno`),
  - 2 on parse error. */
int life_rle_lex(struct LifeRle *);
int life_rle_read(struct LifeRle *, FILE *);
void life_rle_write(FILE *, struct LifeRle);

void *push_token(DArray *, union Tokenizable, int);

void write_tokens(struct TokenWriter *, struct RleToken *);
void write_one_token(struct TokenWriter *, struct RleToken);
/*@}*/
#endif
