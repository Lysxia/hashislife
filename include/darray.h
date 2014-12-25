#ifndef DARRAY_H
#define DARRAY_H

#include <stddef.h>

/*! \defgroup darray Generic dynamic array implementation.
  (Well, as generic as C allows)

  Push an arbitrary number of objects in an array with automatic growing.

  These functions (except `da_init()` of course) all assume that the
  array argument has already been initialized with `da_init()`,
  and not yet `da_unpack()`-ed nor `da_destroy()`-ed. */
/*!@{*/
typedef struct {
  char   *array;
  size_t  data_size;
  size_t  array_length;
  size_t  max_length;
} DArray;

//! Initialize empty DArray
void  da_init(DArray *da, size_t data_size);
//! Check whether the array is empty
int   da_is_empty(const DArray *da);
//! Extend the array by one uninitialized block and returns a pointer to it
void *da_alloc(DArray *da);
//! Extend the array and initialize it with the value pointed to by `v`
void *da_push(DArray *da, void *v);
//! Extract the underlying array
void *da_unpack(const DArray *da, size_t *length);
//! Free resources allocated for the array
void  da_destroy(DArray *da);

// Might need a da_append/concat some day
/*!@}*/
#endif
