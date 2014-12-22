#ifndef BITMAPS_H
#define BITMAPS_H

#include "darray.h"
#include "definitions.h"
#include "runlength.h"

/*! \defgroup bmp Cell matrix representation */
/*!@{*/
//! Types of supported maps
enum MapType { RAW, RLE };

//! Run length encoded line
struct RleLine
{
  struct RleToken *tokens;
  int nb_tokens;
  int line_num;
};

typedef struct
{
  struct RleLine *lines;
  int nb_lines;
} RleMap;

//! Tagged union type
typedef struct
{
  union
  {
    RleMap *rle; //!< Run length encoding
    /*!< This encoding uses tokens of binary values `0`, `1`
      to encode a map line by line, possibly skipping empty
      lines (all `0`). */
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

RleMap *align_tokens(struct RleToken *rle);
struct RleToken *rle_flatten(RleMap *rle_m);

BitMap *rle_to_bm(struct LifeRle rle);

void bm_write(FILE *file, BitMap *bm);

//Rle_line *bm_rle_newline(Darray *rle, int line_num);
/*!@}*/
#endif
