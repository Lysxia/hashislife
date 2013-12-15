#include <stdlib.h>
#include <strings.h>
#include "darray.h"

void da_init(Darray *da, size_t data_size)
{
  da->da = NULL;
  *(int *)&da->data_size = data_size;
  da->array_length = 0;

  return da;
}

void *da_append(Darray *da, char *v)
{
  if (da->array_length == 0)
    da->da = malloc(da->data_size);
  else if (!(da->array_length & (da->array_length - 1)))
    da->da = realloc(da->da, 2 * da->array_length * da->data_size);

  char *dest = da->da + da->data_size * da->array_length;

  if (v != NULL)
    memcpy(dest, v, da->data_size);

  da->array_length++;

  return dest;
}

void da_clear(Darray *da)
{
  if (da->array_length > 0)
    free(da->da);
  da->da = NULL;
  da->array_length = 0;
}
