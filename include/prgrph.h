#ifndef MATRIX_H
#define MATRIX_H
#include <stdlib.h>
#include <stdio.h>

// Paragraph: an array of null-terminated strings
typedef struct Prgrph
{
  char **prgrph;
  int    m;
} Prgrph;

// void **
void   *alloc_prgrph(int m, int n, size_t size);
Prgrph   read_prgrph(FILE *file);
void    write_prgrph(FILE *file, Prgrph prgrph);
void     free_prgrph(Prgrph prgrph);

#endif
