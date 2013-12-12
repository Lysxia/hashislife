#ifndef READ_GOL_H
#define READ_GOL_H

typedef int rule;

typedef struct BitMap BitMap;

enum MapType { RLE };

struct BitMap
{
  enum MapType map_type;
  int x, y, r;
  int **map;
};

int **read_gol(int *m, int *n, rule *r, FILE *file);
void print_matrix(int **map, int m, int n, FILE *file);

int **alloc_matrix(int m, int n);
void free_matrix(int **map, int m);

#endif
