#include <stdlib.h>
#include <stdio.h>
#include "hashtbl.h"

Hashtbl hashtbl_new()
{
  Hashtbl h = {
    .size = init_size,
    .tbl = calloc(init_size*sizeof(Quad*)),
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

Quad* hashtbl_find(Hashtbl hashtbl, int h, Node key)
{
  return list_find (key,hashtbl.tbl[h]);
}

void hashtbl_add(Hashtbl hashtbl, int h, Quad* elt)
{
  elt->tl = hashtbl.tbl[h];
  hashtbl.tbl[h] = elt;
}

Quad* list_find(Quad* list, Node key)
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
