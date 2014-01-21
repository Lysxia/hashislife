#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "runlength.h"
#include "definitions.h"
#include "darray.h"
#include "parsers.h"

#define RLE_LINE_LENGTH 100

int rle_token(FILE *new_file, char *tag);

int rle_token(FILE *new_file, char *tag)
{
  static FILE *file = NULL;
  static char buff[RLE_LINE_LENGTH];
  static int i;

  if ( new_file != NULL )
  {
    file = new_file;
    buff[0] = '\0';
    i = 0;
  }
  else if ( file == NULL )
    return -1;

  i += strspn(buff + i, " \n\t");

  if ( buff[i] == '\0' )
  {
    i = 0;
    if ( fgets(buff, RLE_LINE_LENGTH, file) == NULL )
      return -1;
  }
  else if ( buff[i] == '!' )
    return 0;

  int len;

  len = atoi(buff + i);
  i += strspn(buff +i, "0123456789");
  *tag = buff[i++];

  return len ? len : 1;
}

int read_rle_(FILE *file, Rle *rle);

Rle *read_rle(FILE *file)
{
  char buff[RLE_LINE_LENGTH];

  do
  {
    if ( fgets(buff, RLE_LINE_LENGTH, file) == NULL )
    {
      fprintf(stderr, "read_rle(): Error on input\n");
      return NULL;
    }
  }
  while ( buff[0] == '#' );

  char s[22];
  Rle *rle = malloc(sizeof(Rle));

  if ( rle == NULL )
  {
    perror("read_rle()");
    return NULL;
  }

  switch ( sscanf(buff, "x = %d , y = %d , r = %21s ",
                        &rle->rle_meta.rle_x,
                        &rle->rle_meta.rle_y,
                        s) )
  {
    case 2:
      rle->rle_meta.rle_r = 0;
      break;
    case 3:
      if ( (rle->rle_meta.rle_r = parse_rule(s)) != (rule) -1 )
        break;
    case EOF:
    case 1:
      free(rle);
      fprintf(stderr, "read_rle(): Bad format\n");
      return NULL;
  }

  if ( !read_rle_(file, rle) )
  {
    free(rle);
    return NULL;
  }

  return rle;
}

void rle_push_line_(Darray *lines, Darray *cur_line, int cur_run,
                   int cur_run_alive, int line_num);
void rle_abort_(Darray *lines, Darray *cur_line);

int read_rle_(FILE *file, Rle *rle)
{
  int i = 0, cur_run = 0, cur_run_alive = 0, new_run_alive = 0;

  Darray *lines = da_new(sizeof(struct Rle_line));
  Darray *cur_line = da_new(sizeof(int));

  int run_len;
  char tag;

  // Initializes rle_token
  // Subsequent calls pass NULL
  run_len = rle_token(file, &tag);

  while ( run_len )
  {
    if ( run_len < 0 )
    {
      fprintf(stderr, "read_rle(...): File has invalid format\n");
      rle_abort_(lines, cur_line);
      return 0;
    }

    switch ( tag )
    {
      case NEWLINE_RLE_TOKEN:
        rle_push_line_(lines, cur_line, cur_run,
                       cur_run_alive, i);
        cur_line = da_new(sizeof(int));
        i += run_len;
        cur_run = 0;
        cur_run_alive = 0;
        break;
      case ALIVE_RLE_TOKEN:
        new_run_alive = 1; // fall through
      case DEAD_RLE_TOKEN:
        if ( new_run_alive ^ cur_run_alive )
        {
          da_push(cur_line, (char *) &cur_run);
          cur_run_alive = !cur_run_alive;
          cur_run = run_len;
        }
        else
          cur_run += run_len;
        new_run_alive = 0;
        break;
      default:
        fprintf(stderr, "read_rle(): Unrecognized token\n");
        rle_abort_(lines, cur_line);
        return 0;
    }
    
    run_len = rle_token(NULL, &tag);
  }

  rle_push_line_(lines, cur_line, cur_run,
                 cur_run_alive, i);

  rle->rle_lines = da_unpack(lines, &rle->rle_lines_c);

  return 1;
}

void rle_push_line_(Darray *lines, Darray *cur_line, int cur_run,
                   int cur_run_alive, int line_num)
{
  if ( cur_run_alive )
    da_push(cur_line, (char *) &cur_run);

  struct Rle_line new_line;
  new_line.line_num = line_num;
  new_line.line = da_unpack(cur_line, &new_line.line_length);

  if ( new_line.line_length > 0 )
    da_push(lines, (char *) &new_line);
}

void rle_abort_(Darray *lines, Darray *cur_line)
{
  int k, len;

  free(da_unpack(cur_line, &len));
  struct Rle_line *rle_lines = da_unpack(lines, &len);

  for ( k = 0 ; k < len ; k++)
    free(rle_lines[k].line);

  free(rle_lines);
}

/**/

void bounded_lines(int run_count, char tag, FILE *new_file);

void write_rle(FILE *file, Rle *rle)
{
  int l, c;
  int prev_line_num = 0;

  bounded_lines(0, '\0', file);

  for ( l = 0 ; l < rle->rle_lines_c ; l++ )
  {
    bounded_lines(rle->rle_lines[l].line_num - prev_line_num,
                  NEWLINE_RLE_TOKEN, NULL);

    for ( c = 0 ; c < rle->rle_lines[l].line_length ; c++ )
      bounded_lines(rle->rle_lines[l].line[c],
                    c % 2 ? ALIVE_RLE_TOKEN : DEAD_RLE_TOKEN, NULL);

    prev_line_num = rle->rle_lines[l].line_num;
  }
  printf("\n!\n");
  fflush(file);
}

void bounded_lines(int run_count, char tag, FILE *new_file)
{
  static FILE *file = NULL;
  static char a[20];
  static int line;

  if ( new_file != NULL )
  {
    file = new_file;
    line = 0;
  }
  else if ( file == NULL ) // no file to print to
    return;

  int written = itoa(a, run_count);

  if ( run_count == 1 )
    a[0] = tag;
  else if ( run_count > 1 )
  {
    a[written] = tag;
    a[written+1] = '\0';
    written++;
  }
  else
    return;

  if ( (line += written) > 70 )
  {
    printf("\n");
    line = 0;
  }
  
  printf("%s", a);
}

/**/

void free_rle(Rle *rle)
{
  int l;
  for ( l = 0 ; l < rle->rle_lines_c ; l++ )
    free(rle->rle_lines[l].line);
  free(rle->rle_lines);
  free(rle);
}
