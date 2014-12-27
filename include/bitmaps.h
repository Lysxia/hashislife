#ifndef BITMAPS_H
#define BITMAPS_H

#include "darray.h"
#include "definitions.h"
#include "runlength.h"

/*! \defgroup bmp Cell matrix representation */
/*!@{*/
//! Types of supported maps
enum MapType { MAT, RLE };

//! Run length encoded line
struct RleLine
{
  struct RleToken *tokens;
  size_t nb_tokens;
  size_t line_num; //!< Can be larger than `.nb_tokens`
};

struct RleMap
{
  struct RleLine *lines;
  size_t nb_lines;
};

//! Tagged union type
typedef struct
{
  union
  {
    struct RleMap rle; //!< Run length encoding. More below.
    char         **mat; //!< Raw matrix
  } map;
  /*!< `.map.rle`: This encoding uses `int` tokens of binary values
    `0`, `1` to encode a map line by line, possibly skipping empty
    lines (all `0`). */

  enum MapType map_type; //!< Type tag

  int x; //!< x increasing towards the right
  int y; //!< y towards the bottom
  int corner_x, corner_y; //!< position of top-left corner
  rule r;
} BitMap;

/* Create */
int bm_new_rle(BitMap *, struct LifeRle);
void bm_new_mat(BitMap *, char **, int, int); //!< Create matrix
void **matrix_new(size_t, int, int);

/* Destroy */
void bm_delete(BitMap *map); //!< Destroy map
void RleMap_delete(struct RleMap rle);
void matrix_delete(void **);

/* Write */
void matrix_write(FILE *, char **, int, int);
void bm_write(FILE *, BitMap *);

/* Translate */
int align_tokens(struct RleMap *, struct RleToken *rle);
struct RleToken *rle_flatten(struct RleMap rle_m);
/*!@}*/
#endif
