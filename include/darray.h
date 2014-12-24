#ifndef DARRAY_H
#define DARRAY_H

#include <stdlib.h>

/*! \defgroup darray (Generic) dynamic array implementation. (Well, as generic as C allows) */
// Implements a stack
// push elements, then recover the array of pushed elements
/*!@{*/
typedef struct {
  char   *array;
  size_t  data_size;
  size_t  array_length;
  size_t  max_length;
} DArray;

//! Initialize empty DArray
int   da_init(size_t data_size, DArray *da);
//! Extend the array by one uninitialized block and returns a pointer to it
void *da_alloc(DArray *da);
//! Extend the array and initialize it with the value pointed to by `v`
void *da_push(DArray *da, void *v);
//! Extract the underlying array
void *da_unpack(DArray *da, size_t *length);
/*! One line wrapper around `free()` */
void  da_destroy(DArray *da);

// Might need a da_append/concat some day
/*!@}*/
#endif
