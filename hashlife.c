#include <stdio.h>
#include <stdlib.h>
#include "hashtbl.h"
#include "hashlife.h"

#define DEBUG

const int quad_block_size = 2048;
const int stack_max_size = 1048576;

Hashtbl htbl = NULL;

/* Th e address of a leaf is 0123 representing the 4 bit picture
 * 0 1
 * 2 3 */
Quad *leaves = NULL;

Quad *quad_block;
int count = 1024;

Quad **stack = NULL;
int stack_size = 0;

// Hashtable must have been initialized through hashlife_init
Quad* hashlife(int** map, int m, int n, int mmin, int mmax, int nmin, int nmax, int d)
{
  if (d==0)
  {
    int i, j, acc=0;
    for (i=0 ; i<2 ; i++)
      for (j=0 ; j<2 ; j++)
      {
        acc <<= 1;
        if (mmin+i>=0 && mmin+i<m && nmin+j>=0 && nmin+j<n)
          acc += map[mmin+i][nmin+j];
      }
    return leaves+acc;
  }
  else
  {
    Quad *quad[4];
    int mmid = (mmin+mmax)/2, nmid = (nmin+nmax)/2;

    quad[0] = hashlife(map,m,n, mmin,mmid, nmin,nmid, d-1);
    quad[1] = hashlife(map,m,n, mmin,mmid, nmid,nmax, d-1);
    quad[2] = hashlife(map,m,n, mmid,mmax, nmin,nmid, d-1);
    quad[3] = hashlife(map,m,n, mmid,mmax, nmid,nmax, d-1);

    return mk_quad(quad,d);
  }
}

const int subtrees[5][4][2] = {
  {{0,1},{1,0},{0,3},{1,2}},
  {{0,2},{0,3},{2,0},{2,1}},
  {{0,3},{1,2},{2,1},{3,0}},
  {{1,2},{1,3},{3,0},{3,1}},
  {{2,1},{3,0},{2,3},{3,2}}
};

const int subtrees2[4][4] = {
  {0,4,5,6},
  {4,1,6,7},
  {5,6,2,8},
  {6,7,8,3}
};

// Prerequisite : the four sub trees were computed and hashed...
Quad* mk_quad(Quad *quad[4], int d)
{
  int i,h;

  h = hash(quad);

  // Check if we didn't already memoize requested node
  Quad *q = hashtbl_find(htbl,h,quad);

  if (q==NULL)
  {
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

    // Unwrap quad tree
    Quad *qs[4][4], *q1[9], *tmp[4], **nxt=q->node.n.sub;

    int i,j;

    // qs is the array of depth d-2 subtrees
    // 00 01 10 11
    // 02 03 12 13
    // 20 21 30 31
    // 22 23 32 33
    // q1 represents step 2^(d-1)
    // 0 4 1
    // 5 6 7
    // 2 8 3
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
      q1[i+4] = mk_quad(tmp,d-2)->node.n.next;
    }

    // nxt=q->node.n.sub holds the quad tree pointer to step 2^d
    for (i=0 ; i<4 ; i++)
    {
      for (j=0 ; j<4 ; j++)
        tmp[i] = q1[subtrees2[i][j]];
      nxt[i] = mk_quad(tmp,d-2)->node.n.next;
    }

    q->node.n.next = mk_quad(nxt,d-1);
    q->depth = d;
    
    hashtbl_add(htbl,h,q);
  }

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
          Quad *quad[4];
          int j;

          for (j=0 ; j<4 ; j++)
            quad[j] = leaves+k[j];

          quad_d1(quad,rule);
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

void quad_d1(Quad* quad[4], int rule[16])
{
  Quad *q = malloc(sizeof(Quad));
  int acc = 0, i;

  for (i=0 ; i<4 ; ++i)
  {
    int j, sum = 0;

    q->node.n.sub[i] = quad[i];

    acc <<= 1;
    
    // Count alive neighbors
    for (j=0 ; j<8 ; ++j)
      sum += quad[coord[i][j][0]]->node.l.map[coord[i][j][1]];
    
    if (quad[pos[i][0]]->node.l.map[pos[i][1]])
      acc += rule[sum+8];
    else
      acc += rule[sum];
  }

  q->node.n.next = leaves + acc;
  q->depth = 1;
  q->tl = NULL;

  hashtbl_add(htbl, hash(quad), q);
}

void hash_info()
{
  htbl_stat(htbl);
}

int* step(int state[4])
{
  Quad *quad[4];
  int i;

  for (i=0 ; i<4 ; i++)
    quad[i] = leaves+state[i];

  Quad *q = hashtbl_find(htbl,hash(quad),quad);
  return q->node.n.next->node.l.map;
}
