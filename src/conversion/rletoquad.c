#include "conversion.h"
#include "conversion_aux.h"
#include "hashtbl.h"

/*! Return `NULL` on failure. */
Quad *rle_to_quad(Hashtbl *htbl, const struct RleMap *rle_m)
{
  struct RleMap q_rle_m;
  if ( zip_adjacent_lines(&q_rle_m, rle_m, param_leaf) )
    return NULL;
  Quad *q = condense(htbl, &q_rle_m);
  RleMap_delete(&q_rle_m);
  return q;
}

/*! Return `NULL` on failure. */
Quad *condense(Hashtbl *htbl, struct RleMap *q_rle_m)
{
  if ( q_rle_m->nb_lines == 0 )
    return dead_space(htbl, 0);

  for ( int d = 1
      ; q_rle_m->nb_lines > 1
     || q_rle_m->lines[0].line_num > 0
     || q_rle_m->lines[0].nb_tokens > 1
     || q_rle_m->lines[0].tokens[0].repeat > 1
      ; d++ )
  {
#if 0
    if ( d > 20) exit(0);
    RleMap_write(q_rle_m); fflush(stdout);
#endif
    struct RleMap tmp;
    if ( zip_adjacent_lines(&tmp, q_rle_m, param_cons_with(htbl, d)) )
      return NULL;
    RleMap_delete(q_rle_m);
    *q_rle_m = tmp;
  }

  return q_rle_m->lines[0].tokens[0].value.ptr_;
}

