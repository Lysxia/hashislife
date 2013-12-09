#ifndef HBITMAPS_H
#define HBITMAPS_H

// Assume the remaining cells are dead cells 
// A more general setting could be imagined... Toric map...
Quad *map_to_quad(Hashtbl *htbl, int **map, int mlen, int nlen);

// Draw the map described by q at the specified location
void quad_to_map(int **map, int m, int n,
                 int mlen, int nlen,
                 BigInt mmin, BigInt nmin, Quad *q);

#endif
