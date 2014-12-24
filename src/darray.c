#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "darray.h"

/*! Allocates a starting array.

  \param data_size Size of objects to be stored in the array.
                   Must be positive.
  \param da Pointer to the structure to be initialized.

  Return 0 if successful, 1 otherwise. */
int da_init(size_t data_size, DArray *da)
{
  const size_t da_init_max_len = 8;
  da->data_size = data_size;
  da->array_length = 0;
  da->max_length = da_init_max_len;
  da->array = malloc(da_init_max_len * data_size);

  return ( NULL == da->array );
}

/*! Assumes `da->array` has already been allocated
  (as is done by `da_init()`).

  Return NULL in case of failure. */
void *da_alloc(DArray *da)
{
  if ( da->array_length == da->max_length )
  {
    da->max_length *= 2;
    da->array = realloc(da->array, da->max_length * da->data_size);
    if ( NULL == da->array )
      return NULL;
  }
  return da->array + da->data_size * da->array_length++;
}

/*! Assumes `da->array` has already been allocated
  (as is done by `da_init()`).

  \param v Pointer to an object of size `da->data_size`.

  Return NULL in case of failure. */
void *da_push(DArray *da, void *v)
{
  char *dest = da_alloc(da);
  if ( NULL == dest )
    return NULL;
  return memcpy(dest, (char *) v, da->data_size);
}

/*!
  \param length Pointer to a location where to store the length.
                Possibly NULL, in which case the value is just forgotten.
                (E.g. if you just need a null-terminated string.)

  The array is resized to the given length.
  Return NULL on failure. (That should happen only with very odd
  implementations of `realloc()` though...
  */
void *da_unpack(
  DArray *da,
  size_t *length)
{
  if ( NULL != length )
    *length = da->array_length;
  return realloc(da->array, da->array_length * da->data_size);
}

/*! Free the array */
void da_destroy(DArray *da)
{
  free(da->array);
}

