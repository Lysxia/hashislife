#ifndef DARRAY_H
#define DARRAY_H

/* (Generic) dynamic array implementation. (Well, as generic as C allows) */

typedef struct Darray {
  char *da;
  const int data_size;
  int array_length;
} Darray;

void da_init(Darray *da, size_t data_size);

void *da_append(Darray *da, char *v);

void da_clear(Darray *da);

#endif
