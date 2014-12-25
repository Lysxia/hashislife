#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "darray.h"

/*! Allocates a starting array.

  \param da Pointer to the structure to be initialized.
  \param data_size Size of objects to be stored in the array.
*/
void da_init(DArray *da, size_t data_size)
{
  *da = (DArray) {
    .array = NULL,
    .data_size = data_size,
    .array_length = 0,
    .max_length = 0,
  };
}

int da_is_empty(const DArray *da)
{
  return ( 0 == da->array_length );
}

/*! Return a pointer to the newly allocated object (but uninitialized).
  Return `NULL` in case of failure, then `da` is left intact. */
void *da_alloc(DArray *da)
{
  if ( da->array_length == da->max_length )
  {
    void *tmp;
    if ( NULL == da->array ) // Empty array
    {
      const size_t da_init_max_len = 8;
      da->max_length = da_init_max_len;
      tmp = malloc(da_init_max_len * da->data_size);
      if ( NULL == tmp )
        return NULL;
    }
    else
    {
      da->max_length *= 2;
      tmp = realloc(da->array, da->max_length * da->data_size);
    }
    if ( NULL == tmp )
      return NULL;
    da->array = tmp;
  }
  return da->array + da->data_size * da->array_length++;
}

/*!
  \param v Pointer to an object of size `da->data_size`.

  Return a pointer to the new object, initialized with the contents at `v`.
  Return `NULL` in case of failure, then `da` is left intact. */
void *da_push(DArray *da, void *v)
{
  char *dest = da_alloc(da);
  if ( NULL == dest )
    return NULL;
  return memcpy(dest, (char *) v, da->data_size);
}

/*!
  \param a Pointer to a location where to store the array.
  \param length Pointer to a location where to store the length.
                Possibly NULL, in which case the value is just forgotten.
                (E.g. if you just need a null-terminated string.)

  The array is resized to the given length.
  If the array is empty, set `a` to `NULL` after `free()`-ing the array
  (unless it was already `NULL`).

  Return 0 on success.
  Return 1 on failure; the resizing failed (that should happen
  only with very odd implementations of `realloc()` though...);
  the original array is not freed, and the references have undefined values.

  The type `DArray` argument is left untouched, although the pointer
  may now contain an invalid address.

  In this implementation, `length` is unconditionally set to the array length
  and `a` is set to `NULL` in case of failure, but use the return value
  for error handling.
*/
int da_unpack(
  const DArray *da,
  void **a,
  size_t *length)
{
  *length = da->array_length;
  if ( 0 == *length ) // Then the array is assumed to be NULL
  {
    if ( NULL != da->array )
      free(da->array);
    *a = NULL;
    return 0;
  }
  else
  {
    *a = realloc(da->array, da->array_length * da->data_size);
    return ( NULL == *a );
  }
}

/*! Free the array if it's not `NULL`. */
void da_destroy(DArray *da)
{
  if ( NULL != da->array )
    free(da->array);
}

