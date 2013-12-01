#include "hashtbl.h"
#include "hashlife.h"

// Generalizable to bigger numbers
Quad *effect(Quad *q, int t)
{
  int d = 0, s = 2;

  while (s < t)
  {
    s *= 2;
    d++;
  }

  Quad *ds;
  int d_ = q->depth;

  while (d_ < d)
  {
    ds = dead_space(d_);

    Quad *quad[4] = {q, ds, ds, ds};

    q = cons_quad(quad, ++d_);
  }

  ds = dead_space(d);
  
  Quad *quad[4], *tmp[4] = {ds, ds, ds, ds};

  int i;
  for (i = 0 ; i < 4 ; i++)
  {
    tmp[4 - i] = q;
    quad[i] = destiny(tmp, t);
    tmp[4 - i] = ds;
  }

  
}
