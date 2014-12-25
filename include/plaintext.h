#ifndef PLAINTEXT_H
#define PLAINTEXT_H

#include "runlength.h"

extern FILE *plaintextin;

//! Parse a file in *plaintext* format
/*! The argument must be a non-`NULL` pointer to `struct LifeRle`.
  It is initialized with a runlength encoding of the file contents.

  In case of failure the fields of the argument are undefined.

  Return values:
  - 0 on success;
  - 1 on other errors (see `errno`),
  - 2 on parse error. */
int plaintextlex(struct LifeRle *);

#endif
