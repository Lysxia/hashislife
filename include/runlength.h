#ifndef RUNLENGTH_H
#define RUNLENGTH_H

#include <stdio.h>
#include <stdlib.h>

#include "darray.h"
#include "definitions.h"
#include "parsers.h"

/*! \defgroup rle Run length encoding */
/*!@{*/
#define     END_RLE_TOKEN '!'
#define    DEAD_RLE_TOKEN 'b'
#define   ALIVE_RLE_TOKEN 'o'
#define NEWLINE_RLE_TOKEN '$'

//! Polymorphism!
/*! A union for RleToken */
union Tokenizable
{
  char char_;
  int int_;
  void *ptr_;
};

struct RleToken
{
  union Tokenizable value;
  int repeat;
};

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
struct LifeRle life_rle_lex();
struct LifeRle life_rle_read(FILE *);
void           life_rle_write(FILE *, struct LifeRle);

void push_token(Darray *, union Tokenizable, int);

void write_tokens(struct TokenWriter *, struct RleToken *);
void write_one_token(struct TokenWriter *, struct RleToken);
/*@}*/
#endif
