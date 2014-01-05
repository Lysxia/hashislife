#ifndef PLAINPARSER_H
#define PLAINPARSER_H

#define ALIVE 'o'
#define DEAD '.'

BitMap *read_plain(FILE *file);

void write_plain(FILE *file, BitMap *bm);

void write_matrix(FILE *file, char **map, int m, int n);

void free_matrix(char **map, int m);

#endif
