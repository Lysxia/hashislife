#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "parsers.h"
#include "bitmaps.h"

rule parse_digit_string(char *buff, int *i)
{
  rule r = 0;

  while ( '0' <= buff[*i] && buff[*i] <= '8' )
  {
    r |= 1 << (buff[(*i)++] - '0');
  }

  return r;
}

rule parse_rule(char *buff)
{
  int i = 1;
  rule r;
  char c;

  c = buff[0];

  rule b = parse_digit_string(buff, &i);

  if ( b >> 9 )
    return -1;

  if ( c == 's' || c == 'S' )
    b <<= 9;
  else if ( c != 'b' && c != 'B' )
    return -1;

  // b rule
  if ( buff[i] != '/' )
    return -1;

  c = buff[i+1];
  i += 2;

  rule s = parse_digit_string(buff, &i);

  if ( s >> 9 )
    return -1;

  if ( c == 's' || c == 'S' )
    s <<= 9;
  else if ( c != 'b' && c != 'B' )
    return -1;

  return s | b;
}

#ifdef DEBUG

void test_rle_token(char *filename)
{
  FILE *file;
  if ( (file = fopen(filename, "r")) == NULL )
    return;

  char tag;
  int len = rle_token(file, &tag);

  while ( len )
  {
    if ( len == -1 )
    {
      printf("///\n");
      return;
    }
    printf("%d%c\n", len, tag);
    len = rle_token(NULL, &tag);
  }

  printf("!\n");
}

#endif
