#include <stdlib.h>
#include <stdio.h>
#include "hashtbl.h"

typedef struct QuadList QuadList;

struct Hashtbl
{
  QuadList **tbl;
  int size;
  int count;
  Quad **dead_quad;
  int dead_size;
};

struct QuadList
{
  Quad *head;
  struct QuadList *tail;
};

typedef struct QuadMap QuadMap;
struct QuadMap
{
  int k;
  Quad *v;
  QuadMap *map_tail;
};

/*** Auxiliary functions ***/

void hashlife_init();
int hash(Quad*[4]);
Quad* alloc_quad();
// create depth 1 nodes. Part of hashlife_init() logic.
void quad_d1(Hashtbl *htbl, Quad *quad[4], rule r); 

int hash(Quad *key[4]);
Quad *hashtbl_find(Hashtbl *hashtbl, int h, Quad *key[4]);
void hashtbl_add(Hashtbl *hashtbl, int h, Quad *elt);
Quad *list_find(Quad *key[4], QuadList *list);

const int init_size = 1 << 20; // start size of hashtbl
const int init_dead_size = 32;

/* The address of a leaf is 0123 representing the 4 bit picture
 * 0 1
 * 2 3 */
Quad *leaves = NULL;
const int leaves_count = 16;

#ifdef ALLOC_QUAD
// A smarter (?) memory allocation, malloc chunks of memory
const int quad_block_size = 2 << 11;
const int stack_max_size = 2 << 20;

Quad *quad_block = NULL;
int quad_block_count = 2 << 11;

Quad **stack;
int stack_size = 0;
#endif

/**************************************/

Hashtbl *hashtbl_new(rule r)
{
  if (leaves == NULL) // Must initialize
    hashlife_init();

  Hashtbl *htbl = malloc(sizeof(Hashtbl));

  if (htbl == NULL)
  {
    printf("in hashtbl_new(...): Not enough memory.\n");
    exit(1);
  }
  
  htbl->size = init_size;
  htbl->tbl = malloc(init_size * sizeof(QuadList*));
  htbl->count = 0;

  htbl->dead_quad = malloc(init_dead_size * sizeof(Quad*));
  htbl->dead_size = init_dead_size;

  if (htbl->tbl == NULL || htbl->dead_quad == NULL)
  {
    printf("in hashtbl_new(...): Not enough memory.\n");
    exit(1);
  }

  int i;

  for (i = 0 ; i < init_size ; i++)
    htbl->tbl[i] = NULL;

  htbl->dead_quad[0] = &leaves[0];

  for (i = 1 ; i < htbl->dead_size ; i++)
    htbl->dead_quad[i] = NULL;

  int k[4];

  for (k[0] = 0 ; k[0] < leaves_count ; k[0]++)
    for (k[1] = 0 ; k[1] < leaves_count ; k[1]++)
      for (k[2] = 0 ; k[2] < leaves_count ; k[2]++)
        for (k[3] = 0 ; k[3] < leaves_count ; k[3]++)
        {
          Quad *quad[4];

          int j;
          for (j = 0 ; j < 4 ; j++)
            quad[j] = &leaves[k[j]];

          quad_d1(htbl, quad, r);
        }

  return htbl;
}

void hashtbl_free(Hashtbl *hashtbl)
{
  free(hashtbl->tbl);
  free(hashtbl);
}

Quad *leaf(int k)
{
  return &leaves[k];
}

Quad *dead_space(Hashtbl *htbl, int d)
{
  if (htbl->dead_size <= d)
  {
    const int old_size = htbl->dead_size;
    int i;

    do htbl->dead_size *= 2;
    while (htbl->dead_size <= d);

    Quad **old_quad = htbl->dead_quad;
    
    htbl->dead_quad = malloc(htbl->dead_size * sizeof(Quad*));

    if (htbl->dead_quad == NULL)
    {
      printf("in dead_space(%d): not enough memory.\n", d);
      exit(1);
    }

    for (i = 0 ; i < old_size ; i++)
      htbl->dead_quad[i] = old_quad[i];

    for (i = old_size ; i < htbl->dead_size ; i++)
      htbl->dead_quad[i] = NULL;

    free(old_quad);
  }

  if (htbl->dead_quad[d] == NULL)
  {
    Quad *ds = dead_space(htbl, d-1);
    Quad *zero[4] = {ds, ds, ds, ds};

    return htbl->dead_quad[d] = cons_quad(htbl, zero, d);
  }
  else
    return htbl->dead_quad[d];
}

// Prerequisite : the four sub trees were computed and hashed.
// This is the only constructor of quadtrees to be used
Quad *cons_quad(Hashtbl *htbl, Quad *quad[4], int d)
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

/*** Map functions ***/

Quad *map_assoc(QuadMap *map, int k)
{
  if (map == NULL || map->k > k)
    return NULL;
  else if (map->k == k)
    return map->v;
  else
    return map_assoc(map->map_tail, k);
}

QuadMap* map_add(QuadMap **map, int k, Quad* v)
{
  if (*map == NULL || (*map)->k > k)
  {
    QuadMap *new_map = malloc(sizeof(QuadMap));
    new_map->k = k;
    new_map->v = v;
    new_map->map_tail = *map;
    return *map = new_map;
  }
  else
  {
    return map_add(&(*map)->map_tail, k, v);
  }
}

/*** Memory managing ***/

Quad *alloc_quad()
{
#ifdef ALLOC_QUAD
  if (quad_block_count < quad_block_size)
    return &quad_block[quad_block_count++];
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
#else
  return malloc(sizeof(Quad));
#endif
}

/*** Hashtable function ***/

int hash(Quad *key[4])
{
  long int a[4], x;
  int i;

  for (i = 0 ; i < 4 ; i++)
    a[i] = (long int) key[i] >> 2;

  x = (a[0] << 15) ^ (a[1] << 10) ^ (a[2] << 5) ^ a[3];

  return (unsigned int) x & (init_size - 1);
}

Quad *hashtbl_find(Hashtbl *hashtbl, int h, Quad *key[4])
{
  return list_find(key, hashtbl->tbl[h]);
}

void hashtbl_add(Hashtbl *hashtbl, int h, Quad *elt)
{
  QuadList *ql = malloc(sizeof(QuadList));

  ql->head = elt;
  ql->tail = hashtbl->tbl[h];
  hashtbl->tbl[h] = ql;
  hashtbl->count++;
}

Quad *list_find(Quad *key[4], QuadList *list)
{
  if (list == NULL)
    return NULL;
  else
  {
    int i;
    for (i = 0; i < 4 ; i++)
      if (key[i] != list->head->node.n.sub[i])
        return list_find(key, list->tail);
    return list->head;
  }
}

/*** Initialize hashlife ***/
void hashlife_init()
{
  int i;

  leaves = malloc(leaves_count * sizeof(Quad));

  if (leaves == NULL)
  {
    printf("in hashlife_init(...): Not enough memory. (lol)\n");
    exit(1);
  }

#ifdef ALLOC_QUAD
  stack = malloc(sizeof(Quad*) * stack_max_size);

  if (stack == NULL)
  {
    printf("in hashlife_init(...): Not enough memory. (lol)\n");
    exit(1);
  }
#endif

  for (i = 0 ; i < leaves_count ; i++)
  {
    Quad *n = &leaves[i];

    int j;
    for (j = 0 ; j < 4 ; j--)
      n->node.l.map[j] = (i >> (3 - j)) & 1;
    
    n->depth = 0;
  }
}

/* Depth 1 nodes are computed at the beginning of the program */
// rule : B/S
// Create depth 1 node
void quad_d1(Hashtbl *htbl, Quad *quad[4], rule r)
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

    // Count alive neighbors
    for (j = 0 ; j < 8 ; j++)
      sum += quad[coord[i][j][0]]->node.l.map[coord[i][j][1]];
    
    if (quad[pos[i][0]]->node.l.map[pos[i][1]])
      acc |= ((r >> (sum + 9)) & 1) << (3 - i);
    else
      acc |= ((r >> sum) & 1) << (3 - i);
  }

  q->node.n.next = malloc(sizeof(QuadMap));
  q->node.n.next->k = 0;
  q->node.n.next->v = &leaves[acc];
  q->depth = 1;

  hashtbl_add(htbl, hash(quad), q);
}


/*** Debug functions ***/

void print_quad(Quad *q)
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
  {
    printf("QUAD depth: %d\n", q->depth);

    int i;
    for (i = 0 ; i < 4 ; i++)
      print_quad(q->node.n.sub[i]);
  }
}

int list_length(QuadList *list)
{
  if (list == NULL)
    return 0;
  else 
    return 1 + list_length(list->tail);
}

#define BUCKET_COUNT 8

void htbl_stat(Hashtbl *htbl)
{
  int i, max[BUCKET_COUNT] = {0};
  for (i = 0 ; i < init_size ; i++)
  {
    int l = list_length(htbl->tbl[i]);
    max[l >= BUCKET_COUNT ? BUCKET_COUNT - 1 : l]++;
  }

  printf("LENGTH: %d\n", htbl->count);
  for (i = 0 ; i < BUCKET_COUNT ; i++)
    printf("%d %d\n", i, max[i]);

  return;
}

const int *step(Hashtbl *htbl, int state[4])
{
  Quad *quad[4];
  int i;

  for (i = 0 ; i < 4 ; i++)
    quad[i] = &leaves[state[i]];

  Quad *q = hashtbl_find(htbl, hash(quad), quad);

  return q->node.n.next->v->node.l.map;
}
