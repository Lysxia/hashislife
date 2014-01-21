#include "definitions.h"

int itoa(char *dest, int src)
{
  int i = 0, j;
  do
  {
    dest[i++] = '0' + src % 10;
    src /= 10;
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
