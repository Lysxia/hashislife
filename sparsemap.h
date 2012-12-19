#ifndef SPARSEMAP_H
#define SPARSEMAP_H

#define MIN_LIN 8
#define MIN_COL 8
#define LIMITER '.'
#define DEDCELL ' '
#define ALIVE   '0'

typedef struct
{
    int len, size, **list;
} Smap;

Smap emptysmp();

int open_line(Smap* t, int i);

int add_point(Smap* t, int i_s, int j);

void qadd_point(Smap* t, int i, int j);

Smap smp_of_bmp(int** b, int m, int n);

void print_smap(Smap s, int xM, int xm, int yM, int ym);


void print_line(int* l, int yM, int ym);
int find_line(int** s, int i, int a, int b);
int* new_line(int i, int len);
int find_col(int* s, int j, int a, int b);
int push_point(int** l, int j);
#endif
