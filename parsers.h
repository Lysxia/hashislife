#ifndef READ_GOL_H
#define READ_GOL_H

typedef int rule;

typedef struct BitMap BitMap;
typedef struct BitMap_RLE_line Rle_line;

enum MapType { RLE };

struct BitMap_RLE_line
{
  Darray line_rle;
  int line_num;
};

struct BitMap
{
  union
  {
    Darray rle;
    int **raw;
  } map;

  enum MapType map_type;

  // x increasing towards the right
  // y towards the bottom
  int corner_x, corner_y; // position of top-left corner
  int x, y; // width and height of the map
  int r;
};

void bm_free(BitMap *map);

int rle_token(FILE *new_file, char *tag);

BitMap *read_rle(FILE *file);
void print_rle(BitMap *map, FILE *file);

int **read_gol(int *m, int *n, rule *r, FILE *file);
void print_matrix(int **map, int m, int n, FILE *file);

int **alloc_matrix(int m, int n);
void free_matrix(int **map, int m);

//void test_rle_token(char *filename);

#endif
