#ifndef BIBLOCK_H
#define BIBLOCK_H

#include "bigint.h"

/*! \defgroup biblock Low level definitions for big int implementation. */
/*!@{*/
#define COMPOSITE(a) (a)->digits.composite
#define SIMPLE(a) (a)->digits.simple
#define LENGTH(a) (a)->length
#define MAX_LENGTH(a) (a)->max_length
#define IS_SIMPLE(a) 0 == (a)->max_length
#define IS_COMPOSITE(a) 0 < (a)->max_length
//! Sign bit of a BiBlock
#define SIGN(x) ((x) >> (BiBlock_bit - 1))
//! Uniform view of the representation
#define DIGITS(a) (IS_SIMPLE(a) ? &SIMPLE(a) : COMPOSITE(a))

void bi_block_copy(
  BiBlock *a_digits, const size_t a_length,
  const BiBlock *b_digits, const size_t b_length);

void bi_block_add_to(
  BiBlock *a_digits, const size_t a_length,
  const BiBlock *c_digits, const size_t c_length);

void bi_block_sub_from(
  BiBlock *a_digits, const size_t a_length,
  const BiBlock *c_digits, const size_t c_length);

void bi_block_set(BiBlock *a, const size_t k, const int bit);

size_t bi_block_from_string(BiBlock *a_digits, char *src, const size_t n);
/*!@}*/
#endif
