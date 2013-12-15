#ifndef PARSERS_H
#define PARSERS_H

#include "darray.h"

typedef int rule;

enum MapType { RLE };

typedef struct BitMap
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
} BitMap;

int parse_rule(char *buff);

int **read_gol(int *m, int *n, rule *r, FILE *file);
void print_matrix(int **map, int m, int n, FILE *file);

int **alloc_matrix(int m, int n);
void free_matrix(int **map, int m);

//void test_rle_token(char *filename);

#endif
