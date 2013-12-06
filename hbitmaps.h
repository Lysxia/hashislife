#ifndef HBITMAPS_H
#define HBITMAPS_H

// Assume the remaining of the map is to be filled with dead cells.
// A more general setting could be imagined...
Quad *map_to_quad(Hashtbl *htbl, int **map, int mlen, int nlen);

// Draw the map described by q at the specified location
// It is assumed that map has size [mmax-mmin][nmax-nmin]
void quad_to_map(int **map, int m, int n, int mlen, int nlen,
    BigInt mmin, BigInt nmin, Quad *q);

#endif
