#include <stdlib.h>
#include <stdio.h>
#include "hashtbl.h"

#define BUCKET_COUNT 8

const signed int init_addr_size = 20;
const signed int init_size = 1 << 20;

Hashtbl hashtbl_new()
{
  Hashtbl h = malloc(sizeof(struct Hashtbl));
  h->size = init_size;
  h->tbl = malloc(init_size * sizeof(Quad*));
  h->len = 0;

  int i;

  for (i = 0 ; i < init_size ; i++)
    h->tbl[i] = NULL;

  return h;
}

int hash(Quad* key[4])
{
  long int a[4], x;
  int i;

  for (i = 0 ; i < 4 ; i++)
    a[i] = (long int) key[i] >> 2;

  x = (a[0] << 15) ^ (a[1] << 10) ^ (a[2] << 5) ^ a[3];

  return (unsigned int) x & (init_size - 1);
}

Quad* hashtbl_find(Hashtbl hashtbl, int h, Quad* key[4])
{
  return list_find(key, hashtbl->tbl[h]);
}

void hashtbl_add(Hashtbl hashtbl, int h, Quad* elt)
{
  hashtbl->len++;
  elt->tl = hashtbl->tbl[h];
  hashtbl->tbl[h] = elt;
}

Quad* list_find(Quad* key[4], Quad* list)
{
  if (list != NULL) // left to right lazy evaluation
  {
    int i;
    for (i = 0; i < 4 ; i++)
      if (key[i] != list->node.n.sub[i])
        return list_find(key, list->tl);
    return list;
  }
  else
    return list;
}

void hashtbl_free(Hashtbl hashtbl)
{
  free(hashtbl->tbl);
  free(hashtbl);
}

void print_quad(Quad* q)
{
  if (q->depth == 0)
  {
    int i;
    printf("LEAF ");
    for (i=0 ; i<4 ; i++)
      printf("%d", q->node.l.map[i]);
    printf("\n");
  }
  else
    printf("QUAD depth: %d\n", q->depth);
}

int list_length(Quad* list)
{
  if (list == NULL)
    return 0;
  else 
    return 1 + list_length(list->tl);
}

void htbl_stat(Hashtbl htbl)
{
  int i, max[BUCKET_COUNT]={0};
  for (i = 0 ; i < init_size ; i++)
  {
    int l = list_length(htbl->tbl[i]);
    max[(l>=BUCKET_COUNT) ? BUCKET_COUNT-1 : l]++;
  }

  printf("LENGTH: %d\n", htbl->len);
  for (i = 0 ; i < BUCKET_COUNT ; i++)
    printf("%d %d\n", i, max[i]);

  return;
}
