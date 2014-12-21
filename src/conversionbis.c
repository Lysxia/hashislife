#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "conversion.h"
#include "bigint.h"
#include "hashtbl.h"
#include "darray.h"
#include "prgrph.h"
#include "runlength.h"
#include "conversion_aux.h"

/*** Matrix to- conversion ***/

Quad *rle_to_quad(Hashtbl *htbl, RleMap *rle_m)
{
  return condense(htbl, RleMap_to_QRleMap(rle_m));
}

Quad *condense(Hashtbl *htbl, struct QRleMap qrle_m)
{
}

struct QRleMap RleMap_to_QRleMap(RleMap *rle_m)
{
}

struct LineFuser {
  struct RleLine line[2];
  int i[2];
};

struct QRleLine fuse_RleLines(struct RleLine line[2])
{
  Darray *qtokens = da_new(sizeof(struct QRleToken));
  struct LineFuser lf = {
    .line = {line[0], line[1]},
    .i = {0}
  };
  while ( lf.i[0] < lf.line[0].nb_tokens || lf.i[1] < lf.line[1].nb_tokens )
  {
    int leaf = 0;
    int repeat = -1; // +infinity
    for ( int k = 0 ; k < 2 ; k++ ) // line number
    {
      // Two cells to extract
      if ( i[k] == line[k].nb_tokens ) // no tokens left
        cell = 0;
      else if ( line[k].tokens[i[k]].repeat == 1 )
      {

        cell = line[k].tokens[i[k]].value;
        repeat
          = ( repeat == -1 || line[k].tokens[i[k]].repeat < repeat )
          ? line[k].tokens[i[k]].repeat : repeat;
      }
    }
  }
}

struct RleToken *appariate_tokens(struct RleLine line)
{
  struct RleToken *t = malloc(3 * line.nb_tokens * sizeof(struct RleToken));
  for ( int i = 0, j = 0 ; i < line.nb_tokens ; )
  {
    if ( line.tokens[i].repeat == 1 )
    {
      t[j] = line.tokens[i];
      t[j+1] = ( i + 1 < line.nb_tokens ) 
    }
  }
}

#if 0
struct Quad_rle rle_to_qrle(Rle *rle)
{
  Darray *da = da_new(sizeof(struct Quad_rle_line));

  int i = 0;
  while ( i < rle->rle_lines_c )
  {
    int *line[2], j[2] = {0}, buff[2] = {-1, -1}, l[2], n[2] = {0}, len[2];
    struct Quad_rle_line qrle_l;
    qrle_l.qrle_linenum = rle->rle_lines[i].line_num / 2;
    Darray *da_line = da_new(sizeof(struct Quad_repeat));

    if ( 1 == rle->rle_lines[i].line_num % 2 )
    {
      line[0] = NULL;
      len[0] = 0;
      line[1] = rle->rle_lines[i].line;
      len[1] = rle->rle_lines[i].line_length;
      i++;
    }
    else
    {
      line[0] = rle->rle_lines[i].line;
      len[0] = rle->rle_lines[i].line_length;

      if ( i + 1 < rle->rle_lines_c
        && rle->rle_lines[i+1].line_num == rle->rle_lines[i].line_num + 1 )
      {
        line[1] = rle->rle_lines[i+1].line;
        len[1] = rle->rle_lines[i+1].line_length;
        i += 2;
      }
      else
      {
        line[1] = NULL;
        len[1] = 0;
        i++;
      }
    }

    while ( j[0] < len[0] || j[1] < len[1] )
    {
      int k;
      for ( k = 0 ; k < 2 ; k++ )
        if ( n[k] == 0 )
          n[k] = rle_take_two(line[k], len[k], &j[k], &l[k], &buff[k]);

      struct Quad_repeat qr;

      qr.qr_q = leaf(l[0] << 2 | l[1]);

      if ( n[0] < n[1] )
      {
        qr.qr_n = n[0];
        n[1] -= n[0];
        n[0] = 0;
      }
      else
      {
        qr.qr_n = n[1];
        n[0] -= n[1];
        n[1] = 0;
      }

      da_push(da_line, &qr);
    }

    qrle_l.qrle_line = da_unpack(da_line, &qrle_l.qrle_linelen);
    da_push(da, &qrle_l);
  }

  struct Quad_rle qrle;
  qrle.qrle = da_unpack(da, &qrle.qrle_len);

  return qrle;
}
#endif


