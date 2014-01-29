#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "hashtbl.h"

typedef struct Quad_list Quad_list;
typedef struct Quad_block Quad_block;

struct Hashtbl
{
  int          size;
  int          count;
  int          dead_size;
  Quad       **dead_quad;
  Quad_block  *blocks;
  Quad_list  **tbl;
};

struct Quad_list
{
  Quad       head;
  Quad_list *tail;
};

// A faster memory allocation, malloc chunks of memory
#define BLOCK_MAX_LEN 1048576

struct Quad_block
{
  Quad_block *next_block;
  int         block_len;
  Quad_list   block[BLOCK_MAX_LEN];
};

typedef struct Quad_map Quad_map;
typedef struct Map_block Map_block;

struct Quad_map
{
  Map_block *qm_block;
  int        k;
  Quad      *v;
  Quad_map  *map_tail;
};

struct Map_block
{
  int        m_block_alive;
  Map_block *next_m_block;
  int        m_block_len;
  Quad_map   m_block[BLOCK_MAX_LEN];
};

/*** Auxiliary functions ***/

Quad_list *alloc_quad(Hashtbl *htbl);
Quad_map  *alloc_map();
void       map_block_compact();
Map_block *map_block_compact_(Map_block *);

// create depth 1 nodes. Part of hashlife_init() logic.
void quad_d1(Hashtbl *htbl, Quad *quad[4], rule r); 

int  hash(Quad*[4]);
Quad *hashtbl_find(Hashtbl *htbl, int h, Quad* key[4]);
void hashtbl_add(Hashtbl *htbl, int h, Quad_list *elt);
Quad *list_find(Quad* key[4], Quad_list *list);

void free_block(Quad_block *);
void free_quad(Quad *);
void free_map(Quad_map *); 

/*** Constants and global elements ***/

const int init_size = 1 << 25; // size of hashtbl
const int init_dead_size = 32;

/* The address of a leaf is a 4 digit binary number 0123
 * representing the 4 bit map
 * 0 1
 * 2 3 */
const int  leaves_count = 16;
Quad      *leaves       = NULL;

Map_block *map_blocks = NULL;

/**************************************/

Hashtbl *hashtbl_new(rule r)
{
  if (leaves == NULL) // Must initialize
    hashlife_init();

  Hashtbl *htbl = malloc(sizeof(Hashtbl));

  if (htbl == NULL)
  {
    perror("hashtbl_new()");
    exit(1);
  }

  // Initialize fields
  htbl->size      = init_size;
  htbl->count     = 0;
  htbl->dead_size = init_dead_size;

  htbl->blocks    = malloc(sizeof(Quad_block));
  htbl->tbl       = malloc(init_size * sizeof(Quad_list*));
  htbl->dead_quad = malloc(init_dead_size * sizeof(Quad*));

  if ( !htbl->blocks || !htbl->tbl || !htbl->dead_quad )
  {
    perror("hashtbl_new()");
    exit(1);
  }

  // First block
  htbl->blocks->next_block = NULL;
  htbl->blocks->block_len  = 0;

  int i;

  for ( i = 0 ; i < init_size ; i++ )
    htbl->tbl[i] = NULL;

  htbl->dead_quad[0] = &leaves[0];

  for ( i = 1 ; i < htbl->dead_size ; i++ )
    htbl->dead_quad[i] = NULL;

  int k[4];

  for ( k[0] = 0 ; k[0] < leaves_count ; k[0]++ )
    for ( k[1] = 0 ; k[1] < leaves_count ; k[1]++ )
      for ( k[2] = 0 ; k[2] < leaves_count ; k[2]++ )
        for ( k[3] = 0 ; k[3] < leaves_count ; k[3]++ )
        {
          Quad *quad[4];

          int j;
          for ( j = 0 ; j < 4 ; j++ )
            quad[j] = &leaves[k[j]];

          quad_d1(htbl, quad, r);
        }

  return htbl;
}

void hashtbl_free(Hashtbl *htbl)
{
  free      (htbl->tbl);
  free      (htbl->dead_quad);
  free_block(htbl->blocks);
  free      (htbl);
}

Quad *leaf(int k)
{
  return &leaves[k];
}

Quad *dead_space(Hashtbl *htbl, int d)
{
  if ( htbl->dead_size <= d )
  {
    const int old_size = htbl->dead_size;
    int i;

    do htbl->dead_size *= 2;
    while ( htbl->dead_size <= d );

    htbl->dead_quad = realloc(htbl->dead_quad,
                              htbl->dead_size * sizeof(Quad*));

    if ( !htbl->dead_quad )
    {
      perror("dead_space()");
      exit(1);
    }

    for ( i = old_size ; i < htbl->dead_size ; i++ )
      htbl->dead_quad[i] = NULL;
  }

  if ( !htbl->dead_quad[d] )
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
  if ( quad[0]->depth != quad[1]->depth ||
       quad[0]->depth != quad[2]->depth ||
       quad[0]->depth != quad[3]->depth ||
       quad[0]->depth != d-1 )
    exit(2);

  int h = hash(quad);

  // Check if we didn't already memoize requested node
  Quad *q = hashtbl_find(htbl, h, quad);

  if ( q )
    return q;
  else
  {
    Quad_list *ql = alloc_quad(htbl);

    ql->head.depth = d;
    ql->head.cell_count = NULL;
    ql->head.node.n.next = NULL;

    int i;

    for ( i = 0 ; i < 4 ; i++ )
      ql->head.node.n.sub[i] = quad[i];
 
    hashtbl_add(htbl, h, ql);

    return &ql->head;
  }
}

/*** Initialize hashlife ***/
void hashlife_init(void)
{
  int i;

  leaves = malloc(leaves_count * sizeof(Quad));

  if ( !leaves )
  {
    perror("hashlife_init()");
    exit(1);
  }

  for (i = 0 ; i < leaves_count ; i++)
  {
    Quad *q = &leaves[i];

    int j;
    for ( j = 0 ; j < 4 ; j++ )
      q->node.l.map[j] = (i >> (3 - j)) & 1;

    q->cell_count = NULL;
    q->depth = 0;
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

  Quad_list *ql = alloc_quad(htbl);

  ql->head.depth = 1;
  ql->head.cell_count = NULL;

  int acc = 0, i;

  for ( i = 0 ; i < 4 ; ++i )
  {
    int j, sum = 0; 

    ql->head.node.n.sub[i] = quad[i];

    // Count alive neighbors
    for ( j = 0 ; j < 8 ; j++ )
      sum += quad[coord[i][j][0]]->node.l.map[coord[i][j][1]];
    
    if ( quad[pos[i][0]]->node.l.map[pos[i][1]] )
      acc |= ((r >> (sum + 9)) & 1) << (3 - i);
    else
      acc |= ((r >> sum) & 1) << (3 - i);
  }

  Quad_map *qm = alloc_map();

  if ( !qm )
  {
    perror("quad_d1()");
    exit(1);
  }

  qm->k = 0;
  qm->v = &leaves[acc];
  qm->map_tail = NULL;

  ql->head.node.n.next = qm;

  hashtbl_add(htbl, hash(quad), ql);
}

/*** Map functions ***/

Quad *map_assoc(Quad_map *map, int k)
{
  if ( !map || map->k > k )
    return NULL;
  else if ( map->k == k )
    return map->v;
  else
    return map_assoc(map->map_tail, k);
}

Quad_map *map_add(Quad_map *map, int k, Quad* v)
{
  if ( !map || map->k > k )
  {
    Quad_map *new_map = alloc_map();
    new_map->k = k;
    new_map->v = v;
    new_map->map_tail = map;
    return new_map;
  }
  else
  {
    map->map_tail = map_add(map->map_tail, k, v);
    return map;
  }
}

/*** Memory management ***/

Quad_list *alloc_quad(Hashtbl *htbl)
{
  htbl->count++;

  if ( htbl->blocks->block_len == BLOCK_MAX_LEN )
  {
    Quad_block *new_qb = malloc(sizeof(Quad_block));
    
    if ( !new_qb )
    {
      perror("alloc_quad()");
      exit(1);
    }

    new_qb->block_len = 0;
    new_qb->next_block = htbl->blocks;

    htbl->blocks = new_qb;
  }
  
  return htbl->blocks->block + htbl->blocks->block_len++;
}

Quad_map *alloc_map()
{
  if ( !map_blocks || map_blocks->m_block_len == BLOCK_MAX_LEN )
  {
    Map_block *new_mb = malloc(sizeof(Map_block));

    if ( !new_mb )
    {
      perror("alloc_map()");
      exit(1);
    }

    new_mb->m_block_len   = 0;
    new_mb->m_block_alive = 0;
    new_mb->next_m_block  = map_blocks;

    int i;
    for ( i = 0 ; i < BLOCK_MAX_LEN ; i++ )
      new_mb->m_block[i].qm_block = new_mb;

    map_blocks = new_mb;
  }

  map_blocks->m_block_alive++;

  return map_blocks->m_block + map_blocks->m_block_len++;
}

void map_block_compact()
{
  map_blocks = map_block_compact_(map_blocks);
}

Map_block *map_block_compact_(Map_block *qb)
{
  if ( qb )
  {
    Map_block *next = map_block_compact_(qb->next_m_block);

    if ( qb->m_block_alive )
    {
      qb->next_m_block = next;
      return qb;
    }
    else
    {
      free(qb);
      return next;
    }
  }
  else
    return NULL;
}

/*** Hashtable functions ***/

/*
void binary(intptr_t w)
{
  int k;
  for ( k = 0 ; k < 64 ; k++, w >>= 1 )
    fprintf(stderr, "%d", (w & 1));
  fprintf(stderr, " !\n");
}
*/

int hash(Quad* key[4])
{
  intptr_t a[4], x;
  int i;

  for ( i = 0 ; i < 4 ; i++ )
    a[i] = (intptr_t) key[i];

  x = (a[0] * a[3]) / 1000 ^ (a[2] * a[1]) / 68 ^ (a[1] * a[3] >> 12) ^ (a[0] >> 4);

  /* sample
  static int t = 0;

  t++;

  if ( !(t % 20021) )
  {
    binary(a[0]);
    binary(a[1]);
    binary(a[2]);
    binary(a[3]);
  }
  */

  return (int) x & (init_size - 1);
}

Quad *hashtbl_find(Hashtbl *htbl, int h, Quad* key[4])
{
  return list_find(key, htbl->tbl[h]);
}

void hashtbl_add(Hashtbl *htbl, int h, Quad_list *elt)
{
  elt->tail = htbl->tbl[h];
  htbl->tbl[h] = elt;
}

Quad *list_find(Quad* key[4], Quad_list *list)
{
  if ( !list )
    return NULL;
  else
  {
    int i;
    for ( i = 0 ; i < 4 ; i++ )
      if ( key[i] != list->head.node.n.sub[i] )
        return list_find(key, list->tail);
    return &list->head;
  }
}

void free_block(Quad_block *qb)
{
  while ( qb )
  {
    int i;
    for ( i = 0 ; i < qb->block_len ; i++ )
      free_quad(&qb->block[i].head);

    Quad_block *next = qb->next_block;
    free(qb);
    qb = next;
  }
}

void free_quad(Quad *q)
{
  if ( q->cell_count )
    bi_free(q->cell_count);
  free_map(q->node.n.next);
}

void free_map(Quad_map *qm)
{
  if ( qm )
  {
    qm->qm_block->m_block_alive--;
    qm->qm_block = NULL;
    free_map(qm->map_tail);
    // v member is going to be freed outside as it should be in the hashtbl
  }
}

/*** Debug functions ***/

void print_quad(Quad *q)
{
  if ( q->depth == 0 )
  {
    int i;
    fprintf(stderr, " ");
    for ( i = 0 ; i < 4 ; i++ )
      fprintf(stderr, "%d", q->node.l.map[i]);
    fprintf(stderr, " ");
  }
  else
  {
    fprintf(stderr, "QUAD depth: %d\n", q->depth);

    int i;
    for ( i = 0 ; i < 4 ; i++ )
      print_quad(q->node.n.sub[i]);

    fprintf(stderr, "\n");
  }
}

int list_length(Quad_list *list)
{
  if ( !list )
    return 0;
  else 
    return 1 + list_length(list->tail);
}

#define BUCKET_COUNT 100

void hashtbl_stat(Hashtbl *htbl)
{
  int i, max[BUCKET_COUNT] = {0};
  for ( i = 0 ; i < init_size ; i++ )
  {
    int l = list_length(htbl->tbl[i]);
    max[l >= BUCKET_COUNT ? BUCKET_COUNT - 1 : l]++;
  }

  fprintf(stderr, "LENGTH: %d\n", htbl->count);
  for ( i = 0 ; i < BUCKET_COUNT ; i++ )
  {
    if ( max[i] )
      fprintf(stderr, "%3d %d\n", i, max[i]);
  }

  return;
}

const int *step(Hashtbl *htbl, int state[4])
{
  Quad *quad[4];
  int i;

  for ( i = 0 ; i < 4 ; i++ )
    quad[i] = &leaves[state[i]];

  Quad *q = hashtbl_find(htbl, hash(quad), quad);

  return q->node.n.next->v->node.l.map;
}
