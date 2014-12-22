#ifndef LIFECOUNT_H
#define LIFECOUNT_H

#include "bigint.h"
#include "hashtbl.h"

/* The lifetime of the result is that of the Quad element
 * which itself is that of the hashtable that generated it */
const BigInt *cell_count(Quad *q);

#endif
