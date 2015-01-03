#ifndef BIGINT_H
#define BIGINT_H

#include <stdint.h>
#include <stdlib.h>

/*! \defgroup bigint Big integers
  Except `bi_init()`, these functions assume the structures respect
  the format described for `struct BigInt`.
*/
/*@{*/
//! Unsigned type for bitwise manipulation
/*! We use max width integer type */
typedef uintmax_t BiBlock;

//! Number of bits in the type `BiBlock`
extern const size_t BiBlock_bit;
//! Maximum value of `BiBlock`
extern const BiBlock BiBlock_max;

struct Composite {
  BiBlock *array;
  size_t len; //!< Part of the array in use
};

union Blocks {
  BiBlock simple;
  struct Composite composite;
};

/*! If `.max_len == 0`, `.digits` is `.simple`;
  otherwise `.digits` is `.composite` (already allocated),
  with `.composite.array` pointing to an object of length `.max_len`. */
typedef struct BigInt {
  union Blocks digits;
  size_t max_len; //!< Length of the allocated array (when `.max_len` > 0)
} BigInt;

extern const BigInt * const bi_zero_const;

//! Set a `BigInt` register to an integer value
void bi_simple(BigInt *a, const BiBlock n);

//! Set a register to zero (frees up any dynamically allocated space)
void bi_zero(BigInt *);

//! Reduce the register to a minimal representation of the current value.
int bi_normalize(BigInt *);

//! Check whether the argument contains a representation of zero
int bi_is_zero(const BigInt *);
//! floor(log(bi)) + 1 (minimum number of bits to represent the number)
size_t bi_log2(const BigInt *);

//! Copy a register
int bi_copy(BigInt *a, const BigInt *b);

//! Extract consecutive bits
/*! A number of bits equal to `(sizeof(int) * CHAR_BIT)-1` starting
  from the k-th. */
int bi_slice(const BigInt *a, const size_t k);
//! Extract a single digit
int bi_digit(const BigInt *a, const size_t k);

//! Addition `a = b + c`
int bi_add(BigInt *a, const BigInt *b, const BigInt *c);
//! Convert a `BigInt` to an `int` (if it is small enough)
int bi_to_int(const BigInt *a);

void bi_block_set(BiBlock *a, const size_t k, const int bit);
BiBlock bi_div_int(BigInt *a, BiBlock d);

// Ignores isolated commas
// "10,00", base=10 -> 1000 (0b111010000)
// "10,,00", base=10 -> 10 (0b1010)
//! Read a string
int bi_from_string(BigInt *a, const char *s, const char base);

int bi_to_string(char *dest, const BigInt *a, const char base);

void bi_binary_string(char *dest, const BigInt *a);

void bi_print(const BigInt *a);

char** bi_to_char_mat(
  const BigInt ***bm,
  int m,
  int n,
  int height);
/*!@}*/
#endif
