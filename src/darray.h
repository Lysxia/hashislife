#ifndef DARRAY_H
#define DARRAY_H

/* (Generic) dynamic array implementation. (Well, as generic as C allows) */
// Implements a stack
// push elements, then recover the array of pushed elements

struct Darray;

typedef struct Darray Darray;

Darray *da_new(size_t data_size);

void da_push(Darray *da, char *v);

void *da_unpack(Darray *da, int *length);

#endif
