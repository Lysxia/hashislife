#include <stdlib.h>
#include "conversion_aux.h"

void QRleMap_delete(struct QRleMap qrle_m)
{
  for ( int i = 0 ; i < qrle_m.nb_lines ; i++ )
    free(qrle_m.lines[i].tokens);
  free(qrle_m.lines);
}
