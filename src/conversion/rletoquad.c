#include "conversion.h"
#include "conversion_aux.h"
#include "hashtbl.h"

Quad *rle_to_quad(Hashtbl *htbl, struct RleMap rle_m)
{
  return condense(htbl, fuse_adjacent_lines(rle_m, token_leaf));
}

Quad *condense(Hashtbl *htbl, struct RleMap q_rle_m)
{
  if ( q_rle_m.nb_lines == 0 )
    return dead_space(htbl, 0);

  for ( int d = 0 ;
        q_rle_m.nb_lines > 1
     || q_rle_m.lines[0].nb_tokens > 1
     || q_rle_m.lines[0].tokens[0].repeat > 1 ;
        d++ )
  {
#if 0
    {
      printf("-%d-", qrle.qrle_len);
      int i;
      for ( i = 0 ; i < qrle.qrle_len ; i++ )
        printf("%d %d, ", qrle.qrle[i].qrle_linelen, qrle.qrle[i].qrle_linenum);
    }
#endif
    struct RleMap tmp = q_rle_m;
    q_rle_m = fuse_adjacent_lines(tmp, token_cons_with(htbl));
    RleMap_delete(tmp);
  }

  Quad *q = q_rle_m.lines[0].tokens[0].value.ptr_;

  RleMap_delete(q_rle_m);
 
  return q;
}

union Tokenizable token_cons_with_(void *htbl, union Tokenizable quad_[4])
{
  Quad *quad[4];
  for ( int k = 0 ; k < 4 ; k++ )
    quad[k] = quad_[k].ptr_;
  return (union Tokenizable)
  {
    .ptr_ = (void *) cons_quad((Hashtbl *) htbl, quad, quad[0]->depth+1)
  };
}

struct TokenCurry token_cons_with(Hashtbl *htbl)
{
  return (struct TokenCurry)
  {
    .f = &token_cons_with_,
    .args = htbl,
  };
}

