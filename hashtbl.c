#include <stdlib.h>
#include <stdio.h>
#include "hashtbl.h"

struct Hashtbl {
  int size;
  Quad** tbl;
};

Hashtbl hashtbl_new()
{
  Hashtbl h = {
    .size = init_size,
    .tbl = malloc(init_size*sizeof(Quad*)),
  };
  return h;
}

int hash(Node n)
{
  return (unsigned int) ((long int) n.n.ul
        *(long int) n.n.ur
        *(long int) n.n.bl
        *(long int) n.n.br) >> (sizeof(unsigned int)-init_addr_size);
}

Quad* hashtbl_find(Node key, Hashtbl hashtbl, int h)
{
  return list_find (key,hashtbl.tbl[h]);
}

void hashtbl_add(Quad* elt, Hashtbl hashtbl, int h)
{
  elt->tl = hashtbl.tbl[h];
  hashtbl.tbl[h] = elt;
}

Quad* list_find(Node key, Quad* list)
{
  if (list != NULL && // left to right lazy evaluation
       (key.n.ul != list->node.n.ul ||
        key.n.ur != list->node.n.ur ||
        key.n.bl != list->node.n.bl ||
        key.n.br != list->node.n.br))
    return list_find(key,list->tl);
  else
    return list;
}

void hashtbl_free(Hashtbl hashtbl)
{
  free(hashtbl.tbl);
}
