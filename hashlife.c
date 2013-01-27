#include <stdio.h>
#include <stdlib.h>
#include "hashtbl.h"
#include "hashlife.h"

Hashtbl htbl;
/* The address of a leaf is 0123 representing the 4 bit picture
 * 0 1
 * 2 3 */
Quad* leaves;

Quad* new_quad(Node n, int d)
{
  Quad *q = malloc(sizeof(Quad));
  q->node = n;
  q->depth = d;
  
  hashtbl_add(htbl,hash(n),q);

  return q;
}

/* Depth 1 nodes are computed at the beginning of the program */
/* rule : B/S */
const int coord[4][8][2] = {
  {{0,0},{0,1},{1,0},{0,2},{1,2},{2,0},{2,1},{3,0}},
  {{0,1},{1,0},{1,1},{0,3},{1,3},{2,1},{3,0},{3,1}},
  {{0,2},{0,3},{1,2},{2,0},{3,0},{2,2},{2,3},{3,2}},
  {{0,3},{1,2},{1,3},{2,1},{3,1},{2,3},{3,2},{3,3}}
};

const int pos[4][2] = {{0,3},{1,2},{2,1},{3,0}};

void quad_d1(Node node, int rule[16])
{
  int acc = 0, i;
  Quad *n[4] = {node.n.ul,node.n.ur,node.n.bl,node.n.br};

  for (i=0 ; i<4 ; ++i)
  {
    int j, sum = 0;

    acc <<= 1;
    
    // Count alive neighbors
    for (j=0 ; j<8 ; ++j)
      sum += n[coord[i][j][0]]->node.l.map[coord[i][j][1]];
    
    if (n[pos[i][0]]->node.l.map[pos[i][1]])
      acc += rule[sum+8];
    else
      acc += rule[sum];
  }

  node.n.next = leaves + acc;

  Quad *q = malloc(sizeof(Quad));
  q->node = node;
  q->depth = 1;
  q->tl = NULL;

  hashtbl_add(htbl, hash(node), q);
}


