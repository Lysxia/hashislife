#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "darray.h"

struct Darray {
  char *da;
  size_t data_size;
  int array_length;
};

Darray *da_new(size_t data_size)
{
  Darray *da = malloc(sizeof(Darray));

  if ( da == NULL )
  {
    perror("da_new()");
    exit(1);
  }

  da->da = NULL;
  da->data_size = data_size;
  da->array_length = 0;

  return da;
}

void da_push(Darray *da, char *v)
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
}

void *da_unpack(Darray *da, int *length)
{
  char *a = NULL;

  if ( da->array_length > 0 )
  {
    a = realloc(da->da, da->array_length * da->data_size);
    if ( a == NULL )
    {
      perror("da_unpack()");
      exit(1);
    }
  }
  
  *length = da->array_length;

  free(da);
  return a;
}
