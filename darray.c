#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "darray.h"

void da_init(Darray *da, size_t data_size)
{
  da->da = NULL;
  da->data_size = data_size;
  da->array_length = 0;
}

void *da_append(Darray *da, char *v)
{
  if ( da->array_length == 0 )
    da->da = malloc(da->data_size);
  else if ( !(da->array_length & (da->array_length - 1)) )
  {
    da->da = realloc(da->da, 2 * da->array_length * da->data_size);
    if ( da->da == NULL )
    {
      perror("da_append()");
      exit(1);
    }
  }

  char *dest = da->da + da->data_size * da->array_length;

  if ( v != NULL )
    memcpy(dest, v, da->data_size);

  da->array_length++;

  return dest;
}

void da_clear(Darray *da)
{
  if ( da->array_length > 0 )
    free(da->da);
  da->da = NULL;
  da->array_length = 0;
}
