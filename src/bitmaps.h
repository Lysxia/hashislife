#ifndef BITMAPS_H
#define BITMAPS_H

#include "definitions.h"
#include "darray.h"
/*! \defgroup bmp Cell matrix representation */
/*!@{*/
//! Types of supported map
enum MapType { RAW, RLE };

//! Run length encoded line
typedef struct RleLine
{
  struct {
    int state;
    int repeat;
  }   *encoding;
  int  length;
  int  line_num;
} RleLine;

typedef struct RLE
{
  RleLine *lines;
  int      nb_lines;
} RleMap;

//! Tagged union type
typedef struct
{
  union
  {
    RleMap *rle; //!< Run length encoding
    char  **raw; //!< Raw matrix
  } map;

  enum MapType map_type; //!< Type tag

  int x; //!< x increasing towards the right
  int y; //!< y towards the bottom
  int corner_x, corner_y; //!< position of top-left corner
  rule r;
} BitMap;

BitMap *bm_new(enum MapType t, void *map); //!< Create map
void    bm_delete(BitMap *map); //!< Destroy map

void RleMap_delete(RleMap *rle);
void matrix_delete(void **a, int m);

//Rle_line *bm_rle_newline(Darray *rle, int line_num);
/*!@}*/
#endif
