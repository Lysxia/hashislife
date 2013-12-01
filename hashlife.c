#include <stdio.h>
#include <stdlib.h>
#include "hashtbl.h"
#include "hashlife.h"

#define DEBUG

Quad* alloc_quad();

void quad_d1(Quad* quad[4], rule r);

Hashtbl htbl;

/* The address of a leaf is 0123 representing the 4 bit picture
 * 0 1
 * 2 3 */
Quad *leaves;

Quad **dead_quad; // Squares of dead cells
int dead_size = 32;

// A smarter memory allocation, malloc chunks of memory
// May prevent future garbage collection though...
const int quad_block_size = 2 << 11;
const int stack_max_size = 2 << 20;

Quad *quad_block;
int quad_block_count = 2 << 11;

Quad **stack;
int stack_size = 0;

// Hashtable must have been initialized through hashlife_init

// Prerequisite : the four sub trees were computed and hashed.
// This is the only constructor of quadtrees
Quad* cons_quad(Quad* quad[4], int d)
{
  int h = hash(quad);

  // Check if we didn't already memoize requested node
  Quad *q = hashtbl_find(htbl, h, quad);

  if (q == NULL)
  {
    q = alloc_quad();

    q->depth = d;
    q->node.n.next = NULL;

    int i;

    for (i = 0 ; i < 4 ; i++)
      q->node.n.sub[i] = quad[i];
 
    hashtbl_add(htbl, h, q);
  }

  return q;
}

Quad* alloc_quad()
{
  if (quad_block_count < quad_block_size)
    return quad_block + quad_block_count++;
  else if (stack_size == stack_max_size)
  {
    printf("in alloc_quad(): Not enough stack space.\n");
    exit(1);
  }
  else
  {
    quad_block = malloc(quad_block_size * sizeof(Quad));
    stack[stack_size++] = quad_block;

    if (quad_block == NULL)
    {
      printf("in alloc_quad(): Not enough memory.\n");
      exit(1);
    }

    quad_block_count = 1;

    return quad_block;
  }
}

Quad* fate(Quad* q)
{
  // quad->depth > 0
  const int subtrees[5][4][2] = {
    {{0,1},{1,0},{0,3},{1,2}},
    {{0,2},{0,3},{2,0},{2,1}},
    {{0,3},{1,2},{2,1},{3,0}},
    {{1,2},{1,3},{3,0},{3,1}},
    {{2,1},{3,0},{2,3},{3,2}}
  },
  subtrees2[4][4] = {
    {0,4,5,6},
    {4,1,6,7},
    {5,6,2,8},
    {6,7,8,3}
  };

  if (q->node.n.next == NULL)
  {
    // Unwrap quad tree
    Quad *qs[4][4], *q1[9], *tmp[4], *nxt[4],
         **quad = q->node.n.sub;

    int i, j, d = q->depth;

    /* qs is the array of depth d-2 subtrees
         00 01 10 11
         02 03 12 13
         20 21 30 31
         22 23 32 33
       q1 represents step 2^(d-1)
         0 4 1
         5 6 7
         2 8 3 */
    for (i=0 ; i<4 ; i++)
    {
      for (j=0 ; j<4 ; j++)
        qs[i][j] = quad[i]->node.n.sub[j];
      // here we can immediately get q1[0..3]
      q1[i] = quad[i]->node.n.next;
    }

    // we compute q1[4..8]
    for (i=0 ; i<5 ; i++)
    {
      for (j=0 ; j<4 ; j++)
        tmp[j] = qs[subtrees[i][j][0]][subtrees[i][j][1]];
      q1[i+4] = fate(cons_quad(tmp,d-2));
    }

    // nxt=q->node.n.sub holds the quad tree pointer to step 2^d
    for (i=0 ; i<4 ; i++)
    {
      for (j=0 ; j<4 ; j++)
        tmp[i] = q1[subtrees2[i][j]];
      nxt[i] = fate(cons_quad(tmp,d-2));
    }

    q->node.n.next = cons_quad(nxt,d-1);
  }

  return q->node.n.next;
}

Quad* dead_space(int d)
{
  if (dead_size <= d)
  {
    int i, new_size = dead_size;
    while (new_size <= d)
      dead_size *= 2;

    Quad **new_quad = malloc(new_size * sizeof(Quad*));

    if (new_quad == NULL)
    {
      printf("in dead_space(%d): not enough memory.\n", d);
      exit(1);
    }

    for (i = 0 ; i < dead_size ; i++)
      new_quad[i] = dead_quad[i];

    for (i = dead_size ; i < new_size ; i++)
      new_quad[i] = NULL;

    dead_size = new_size;

    free(dead_quad);

    dead_quad = new_quad;
  }

  if (dead_quad[d] == NULL)
  {
    Quad *ds = dead_space(d-1);
    Quad *zero[4] = {ds, ds, ds, ds};

    return dead_quad[d] = cons_quad(zero, d);
  }
  else
    return dead_quad[d];
}

void hashlife_init(rule r)
{
  const int leaves_n = 16;
  int i;

  htbl = hashtbl_new();

  stack = malloc(sizeof(Quad*) * stack_max_size);
  leaves = malloc(leaves_n * sizeof(Quad));
  dead_quad = malloc(dead_size * sizeof(Quad*));

  if (stack == NULL || leaves == NULL || dead_quad == NULL)
  {
    printf("in hashlife_init(...): Not enough memory. (lol)\n");
    exit(1);
  }

  for (i = 0 ; i < leaves_n ; i++)
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

  dead_quad[0] = &leaves[0];

  for (i = 1 ; i < dead_size ; i++)
    dead_quad[i] = NULL;

  int k[4];

  for (k[0] = 0 ; k[0] < leaves_n ; k[0]++)
    for (k[1] = 0 ; k[1] < leaves_n ; k[1]++)
      for (k[2] = 0 ; k[2] < leaves_n ; k[2]++)
        for (k[3] = 0 ; k[3] < leaves_n ; k[3]++)
        {
          Quad *quad[4];
          int j;

          for (j = 0 ; j < 4 ; j++)
            quad[j] = leaves + k[j];

          quad_d1(quad, r);
        }
}

/* Depth 1 nodes are computed at the beginning of the program */
// rule : B/S
// Create depth 1 node
void quad_d1(Quad* quad[4], rule r)
{
  const int coord[4][8][2] = {
    {{0,0},{0,1},{1,0},{0,2},{1,2},{2,0},{2,1},{3,0}},
    {{0,1},{1,0},{1,1},{0,3},{1,3},{2,1},{3,0},{3,1}},
    {{0,2},{0,3},{1,2},{2,0},{3,0},{2,2},{2,3},{3,2}},
    {{0,3},{1,2},{1,3},{2,1},{3,1},{2,3},{3,2},{3,3}}
  },
  pos[4][2] = {{0,3},{1,2},{2,1},{3,0}};

  Quad *q = alloc_quad();

  int acc = 0, i;

  for (i = 0 ; i < 4 ; ++i)
  {
    int j, sum = 0; 

    q->node.n.sub[i] = quad[i];

    acc <<= 1;
    
    // Count alive neighbors
    for (j=0 ; j<8 ; ++j)
      sum += quad[coord[i][j][0]]->node.l.map[coord[i][j][1]];
    
    if (quad[pos[i][0]]->node.l.map[pos[i][1]])
      acc += (r >> (sum + 9)) & 1;
    else
      acc += (r >> sum) & 1;
  }

  q->node.n.next = leaves + acc;
  q->depth = 1;

  hashtbl_add(htbl, hash(quad), q);
}

void hash_info()
{
  htbl_stat(htbl);
}

const int* step(int state[4])
{
  Quad *quad[4];
  int i;

  for (i=0 ; i<4 ; i++)
    quad[i] = leaves+state[i];

  Quad *q = hashtbl_find(htbl,hash(quad),quad);
  return q->node.n.next->node.l.map;
}
