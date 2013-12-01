#ifndef READ_GOL_H
#define READ_GOL_H

typedef int rule;

int **read_gol(int *m, int *n, rule *r, FILE *file);
void print_map(int **map, int m, int n, FILE *file);

#endif
