#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "prgrph.h"
#include "parsers.h"
#include "darray.h"

void *alloc_prgrph(int m, int n, size_t size)
{
  char **a = malloc(m * sizeof(char *));

  if ( !a )
    return NULL;

  int i;
  for ( i = 0 ; i < m ; i++ )
  {
    a[i] = malloc((n + 1) * size);
    if ( !a[i] )
    {
      for ( i-- ; i >= 0 ; i-- )
        free(a[i]);
      free(a);
      return NULL;
    }
    a[i][n] = '\0';
  }

  return a;
}

const Prgrph prgrph_error = { .prgrph = NULL, .m = -1 };

Prgrph read_prgrph(FILE *file)
{
  if ( !file )
	  return prgrph_error;

  const int buffsize = 81; // 80 char + \n + \0
  char *buff = malloc((buffsize + 1) * sizeof(char));

  if ( !buff )
  {
    perror("read_plain()");
    return prgrph_error;
  }

  do
  {
    if (  fgets(buff, buffsize + 1, file) == NULL )
    {
      perror("read_prgrph()");
      free(buff);
      return prgrph_error;
    }
    else if ( strchr(buff,'\n') == NULL )
    {
      fputs("read_prgrph(): A line must not exceed 80 characters.\n", stderr);
      free(buff);
      return prgrph_error;
    }
  }
  while ( buff[0] == '!' );

  Darray *da = da_new(sizeof(char *));

  do
  {
    int linelen = strlen(buff);

    if ( linelen == buffsize && buff[buffsize-1] != '\n' )
    {
      fputs("read_prgrph(): A line must not exceed 80 characters\n", stderr);
      Prgrph cancel_p;
      cancel_p.prgrph = da_unpack(da, &cancel_p.m);
      free_prgrph(cancel_p);
      return prgrph_error;
    }

    buff[linelen-1] = '\0';
    da_push(da, &buff);

    buff = malloc((buffsize + 1) * sizeof(char));

    if ( !buff )
    {
      perror("read_prgrph()");
      return prgrph_error;
    }
  }
  while ( fgets(buff, buffsize + 1, file) != NULL );

  free(buff);

  Prgrph prgrph;
  prgrph.prgrph = da_unpack(da, &prgrph.m);
  return prgrph;
}

void write_prgrph(FILE *file, Prgrph prgrph)
{
  int i;
  for ( i = 0 ; i < prgrph.m ; i++ )
  {
    fputs(prgrph.prgrph[i], file);
    fputc('\n', file);
  }
  fflush(file);
}

void free_prgrph(Prgrph prgrph)
{
  int i;
  for ( i = 0 ; i < prgrph.m ; i++ )
    free(prgrph.prgrph[i]);
  free(prgrph.prgrph);
}
