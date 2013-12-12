#ifndef READ_GOL_H
#define READ_GOL_H

typedef int rule;

typedef struct BitMap BitMap;

enum MapType { RLE };

struct BitMap
{
  enum MapType map_type;
  int corner_x, corner_y;
  int x, y;
  int r;
  int **map;
  int *line_num, *line_len;
  int len;
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
