#include <assert.h>
#include "definitions.h"

char digit_to_char(int d) {
  return ( d < 10 ) ? '0' + d : ( d < 36 ) ? 'A' + d - 10 : '?';
}

int itoa(char *dest, int src, int base) {
  assert( base < 36 );
  int i = 0, j;
  do
  {
    dest[i++] = digit_to_char(src % base);
    src /= base;
  }
  while ( src > 0 );

  for ( j = 0 ; j < i / 2 ; j++ )
  {
    char tmp = dest[i-j-1];
    dest[i-j-1] = dest[j];
    dest[j] = tmp;
  }

  dest[i] = '\0';

  return i;
}
