#include "conversion.h"
#include "conversion_aux.h"
#include "hashtbl.h"

Quad *rle_to_quad(Hashtbl *htbl, struct RleMap rle_m)
{
  return condense(htbl, zip_adjacent_lines(rle_m, param_leaf));
}

Quad *condense(Hashtbl *htbl, struct RleMap q_rle_m)
{
  if ( q_rle_m.nb_lines == 0 )
    return dead_space(htbl, 0);

  for ( int d = 1
      ; q_rle_m.nb_lines > 1
     || q_rle_m.lines[0].line_num > 0
     || q_rle_m.lines[0].nb_tokens > 1
     || q_rle_m.lines[0].tokens[0].repeat > 1
      ; d++ )
  {
#if 0
    if ( d > 20) exit(0);
    RleMap_write(q_rle_m); fflush(stdout);
#endif
    struct RleMap tmp = q_rle_m;
    q_rle_m = zip_adjacent_lines(tmp, param_cons_with(htbl, d));
    RleMap_delete(tmp);
  }

  Quad *q = q_rle_m.lines[0].tokens[0].value.ptr_;

  RleMap_delete(q_rle_m);

  return q;
}

