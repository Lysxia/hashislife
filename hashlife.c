#include <stdio.h>
#include <stdlib.h>
#include "hashtbl.h"
#include "hashlife.h"

#define DEBUG

const int quad_block_size = 2048;
const int stack_max_size = 1048576;

Hashtbl htbl;

/* Th e address of a leaf is 0123 representing the 4 bit picture
 * 0 1
 * 2 3 */
Quad *leaves;

Quad *quad_block;
int count = 1024;

Quad **stack = NULL;
int stack_size = 0;

Quad* new_quad(Node* n, int d)
{
  Quad *q;
  if (count < quad_block_size)
    q = quad_block+count++;
  else
  {
    if (stack == NULL)
      stack = malloc(sizeof(Quad*)*stack_max_size);
    else if (stack_size == stack_max_size)
    {
      printf("Not enough stack space.");
      exit(1);
    }
    else
      stack[stack_size++] = quad_block;

    quad_block = malloc(quad_block_size*sizeof(Quad));
    count = 1;

    q = quad_block;
  }

  q->node = *n;
  q->depth = d;
  
  hashtbl_add(htbl,hash(n),q);

  return q;
}

void hashlife_init(int rule[16])
{
  const int leaves_n = 16;
  int i;

  htbl = hashtbl_new();
  leaves = malloc(leaves_n*sizeof(Quad));

  for (i=0 ; i<leaves_n ; i++)
  {
    Node n;
    int j, i_ = i;
    for (j=3 ; j>=0 ; j--)
    {
      n.l.map[j] = i_ & 1;
      i_ >>= 1;
    }
    leaves[i].depth = 0;
    leaves[i].node = n;
    leaves[i].tl = NULL;
  }

  int k[4];

  for (k[0]=0 ; k[0] < leaves_n ; k[0]++)
    for (k[1]=0 ; k[1] < leaves_n ; k[1]++)
      for (k[2]=0 ; k[2] < leaves_n ; k[2]++)
        for (k[3]=0 ; k[3] < leaves_n ; k[3]++)
        {
          Node n;
          int j;

          for (j=0 ; j<4 ; j++)
            n.n.sub[j] = leaves+k[j];
          n.n.next = NULL;

          quad_d1(&n,rule);
        }
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

void quad_d1(Node* node, int rule[16])
{
  int acc = 0, i;

  for (i=0 ; i<4 ; ++i)
  {
    int j, sum = 0;

    acc <<= 1;
    
    // Count alive neighbors
    for (j=0 ; j<8 ; ++j)
      sum += node->n.sub[coord[i][j][0]]->node.l.map[coord[i][j][1]];
    
    if (node->n.sub[pos[i][0]]->node.l.map[pos[i][1]])
      acc += rule[sum+8];
    else
      acc += rule[sum];
  }

  node->n.next = leaves + acc;

  Quad *q = malloc(sizeof(Quad));
  q->node = *node;
  q->depth = 1;
  q->tl = NULL;

  hashtbl_add(htbl, hash(node), q);
}

void hash_info()
{
  htbl_stat(htbl);
}

int* step(int state[4])
{
  Node n;
  int i;

  for (i=0 ; i<4 ; i++)
    n.n.sub[i] = leaves+state[i];

  Quad *q = hashtbl_find(htbl,hash(&n),&n);
  return q->node.n.next->node.l.map;
}
