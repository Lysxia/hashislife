#include <stdlib.h>
#include <stdio.h>

#include "bitmaps.h"
#include "quad.h"
#include "conversion.h"
#include "hashtbl.h"
#include "hashtbl_aux.h"

#define N 100

int main(void) {
  hashlife_init();
  Quad q[N];
  for ( int i = 0 ; i < 16 ; i++ )
  {
    for ( int k = 0 ; k < 4 ; k++ )
      q[i].node.n.sub[k] = &leaves[(i + k) % 16];
    q[i].depth = 1;
    q[i].alive = NULL;
  }
  for ( int i = 16 ; i < N ; i++ )
  {
    for ( int k = 0 ; k < 4 ; k++ )
      q[i].node.n.sub[k] = &q[i - (i % 16) - 16 + ((i + k) % 16)];
    q[i].depth = i / 16 + 1;
    q[i].alive = NULL;
  }
  print_quad(&q[19]);
  UMatrix um = quad_to_matrix(&q[32], 0, bi_from_int(3), 4, bi_from_int(3), 4);
  matrix_write(stdout, um.char_, 4, 4);
  return 0;
}

