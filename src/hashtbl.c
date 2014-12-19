/* Quadtree hashconsing */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "hashtbl.h"

/*! \defgroup hashtbl_aux Auxiliary definitions (hashtables) */
/*!@{*/
QuadList *quadlist_alloc(Hashtbl *htbl);
//void       map_block_compact();
//Map_block *map_block_compact_(Map_block *);

//! Create depth 1 node.
void quad_d1(Hashtbl *htbl, Quad *quad[4], rule r); 

int  hash(Quad*[4]);
Quad *hashtbl_find(Hashtbl *htbl, int h, Quad* key[4]);
void hashtbl_add(Hashtbl *htbl, int h, QuadList *elt);
Quad *list_find(Quad* key[4], QuadList *list);

void block_free(QuadBlock *);
void quad_free(Quad *);
//void free_map(Quad_map *); 
/*!@}*/

/*** Constants and global elements ***/

const int init_size = 1 << 25; // size of hashtbl
const int init_dead_size = 32;

/* The address of a leaf is a 4 digit binary number 0123
  representing the 4 bit map
  0 1
  2 3 */
Quad      *leaves = NULL;

Map_block *map_blocks = NULL;

/*! Allocates, initializes and returns a new `Hashtbl` structure.
 
  @param r The initialization precomputes patterns for this rule set.
               (So the same hashtable can not be used for different
               rule sets.)

  \see rules
  \see hashtbl_delete
*/
Hashtbl *hashtbl_new(rule r)
{
  /* The first call initializes global parameters */
  if (leaves == NULL)
    hashlife_init();

  Hashtbl *htbl = malloc(sizeof(Hashtbl));

  if (htbl == NULL)
  {
    perror("hashtbl_new(): Failed to allocate table.");
    exit(1);
  }

  /* Initialize hashtable fields */
  htbl->size      = init_size;
  htbl->count     = 0;
  htbl->tbl       = malloc(init_size * sizeof(QuadList*));

  htbl->dead_size = init_dead_size;
  htbl->dead_quad = malloc(init_dead_size * sizeof(Quad*));

  htbl->blocks    = malloc(sizeof(QuadBlock));

  if ( !htbl->blocks || !htbl->tbl || !htbl->dead_quad )
  {
    perror("hashtbl_new(): Failed to allocate one or several field(s).");
    exit(1);
  }

  /* First memory block */
  htbl->blocks->block_next = NULL;
  htbl->blocks->block_len  = 0;

  int i;

  /* The leaf (depth 0) of all zero bits */
  htbl->dead_quad[0] = &leaves[0]; // The zero leaf should be at index 0

  /* All other cells are left blank (to-be-computed) */
  for ( i = 1 ; i < htbl->dead_size ; i++ )
    htbl->dead_quad[i] = NULL;

  /* Zero-initialize hashtable */
  for ( i = 0 ; i < init_size ; i++ )
    htbl->tbl[i] = NULL;

  /* Insert depth 1 nodes
    There are depth1_count = 16^4 of these, indexed by k4.
    The subtree (leaves) indices are obtained with four bitmasks. */
  int k4;
  for ( k4 = 0 ; k4 < depth1_count ; k4++ )
  {
    Quad *quad[4];
    int j;
    const int four_1 = (1 << 4) - 1;
    for ( j = 0 ; j < 4 ; j++ )
      quad[j] = &leaves[(k4 >> (4*j)) & four_1];
    quad_d1(htbl, quad, r);
  }

  return htbl;
}

/*! \see hashtbl_new */
void hashtbl_delete(Hashtbl *htbl)
{
        free(htbl->tbl);
        free(htbl->dead_quad);
  block_free(htbl->blocks);
        free(htbl);
}

/*! Leaves are indexed from 0 to \link leaves_count \endlink-1 = 15. */
Quad *leaf(int k)
{
  return &leaves[k];
}

Quad *dead_space(Hashtbl *htbl, int d)
{
  if ( htbl->dead_size <= d ) // The dead areas array is too small
  {
    const int old_size = htbl->dead_size;

    /* Increase size until d fits */
    do htbl->dead_size *= 2;
    while ( htbl->dead_size <= d );

    htbl->dead_quad = realloc(htbl->dead_quad,
                              htbl->dead_size * sizeof(Quad*));

    if ( !htbl->dead_quad )
    {
      perror("dead_space(): Failed to reallcoate space.");
      exit(1);
    }

    /* NULL-initialize new cells */
    int i;
    for ( i = old_size ; i < htbl->dead_size ; i++ )
      htbl->dead_quad[i] = NULL;
  }

  if ( !htbl->dead_quad[d] ) // The requested area does not exist yet
  {
    Quad *ds = dead_space(htbl, d-1); // Recursively find its subtrees
    Quad *zero[4] = {ds, ds, ds, ds};

    return htbl->dead_quad[d] = cons_quad(htbl, zero, d);
  }
  else
    return htbl->dead_quad[d];
}

/*! Prerequisite : the four subtrees were computed and hashed.
  
  Use this function to construct new nodes.
*/
Quad *cons_quad(
  Hashtbl *htbl, //!< Current hashtable
  Quad *quad[4], //!< Array of subtrees
  int d)         //!< Depth of current node
{
  assert(quad[0]->depth == quad[1]->depth
      && quad[0]->depth == quad[2]->depth
      && quad[0]->depth == quad[3]->depth
      && quad[0]->depth == d-1);

  int h = hash(quad);

  Quad *q = hashtbl_find(htbl, h, quad);

  if ( q ) // The requested node has been memoized already
    return q;
  else
  {
    QuadList *ql = quadlist_alloc(htbl);

    ql->head.depth = d;
    ql->head.cell_count = NULL;
    ql->head.node.n.skip = NULL;
    ql->head.node.n.short_skip = NULL;

    int i;
    for ( i = 0 ; i < 4 ; i++ )
      ql->head.node.n.sub[i] = quad[i];
 
    hashtbl_add(htbl, h, ql);

    return &ql->head;
  }
}

/*! This is automatically called at the first call of `hashtbl_new()` */
void hashlife_init(void)
{
  // Initialize quadtree leaves
  leaves = malloc(leaves_count * sizeof(Quad));

  if ( !leaves )
  {
    perror("hashlife_init()");
    exit(1);
  }

  int i;
  for (i = 0 ; i < leaves_count ; i++)
  {
    Quad *q = &leaves[i];

    q->depth = 0;
    q->cell_count = NULL;

    int j;
    for ( j = 0 ; j < 4 ; j++ )
      q->node.l.map[j] = (i >> (3 - j)) & 1;
  }
}

/*! Part of hashlife_init() logic.

  Depth 1 nodes and their `skip` field (QInNode; one step progress)
  are computed at hashtable creation (\link hashtable_new() \endlink) */
void quad_d1(
  Hashtbl *htbl,
  Quad    *quad[4], //!< Subtrees (four leaves)
  rule     r)       //!< Rule set
{
  // Coordinates of the neighbors of the four center cells
  const int coord[4][8][2] = {
    {{0,0},{0,1},{1,0},{0,2},{1,2},{2,0},{2,1},{3,0}},
    {{0,1},{1,0},{1,1},{0,3},{1,3},{2,1},{3,0},{3,1}},
    {{0,2},{0,3},{1,2},{2,0},{3,0},{2,2},{2,3},{3,2}},
    {{0,3},{1,2},{1,3},{2,1},{3,1},{2,3},{3,2},{3,3}}
  };
  // Coordinates of the center cells
  const int pos[4][2] = {{0,3},{1,2},{2,1},{3,0}};

  QuadList *ql = quadlist_alloc(htbl);

  ql->head.depth = 1;
  ql->head.cell_count = NULL;

  // Fill skip field
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

  ql->head.node.n.skip = &leaves[acc];

  hashtbl_add(htbl, hash(quad), ql);
}

/*** Map functions ***/
/*
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
*/

/*** Memory management ***/

QuadList *quadlist_alloc(Hashtbl *htbl)
{
  htbl->count++;

  if ( htbl->blocks->block_len == BLOCK_MAX_LEN )
  {
    QuadBlock *new_qb = malloc(sizeof(QuadBlock));
    
    if ( !new_qb )
    {
      perror("quadlist_alloc(): Failed to allocate new block.");
      exit(1);
    }

    new_qb->block_len = 0;
    new_qb->block_next = htbl->blocks;

    htbl->blocks = new_qb;
  }
  
  return htbl->blocks->block + htbl->blocks->block_len++;
}

/*
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
*/

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

void hashtbl_add(Hashtbl *htbl, int h, QuadList *elt)
{
  elt->tail = htbl->tbl[h];
  htbl->tbl[h] = elt;
}

Quad *list_find(Quad* key[4], QuadList *list)
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

void block_free(QuadBlock *qb)
{
  while ( qb )
  {
    int i;
    for ( i = 0 ; i < qb->block_len ; i++ )
      quad_free(&qb->block[i].head);

    QuadBlock *next = qb->block_next;
    free(qb);
    qb = next;
  }
}

// TODO
void quad_free(Quad *q)
{
  if ( q->cell_count )
    bi_free(q->cell_count);
}

/*
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
*/

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

int list_length(QuadList *list)
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

#undef BUCKET_COUNT

const int *step(Hashtbl *htbl, int state[4])
{
  Quad *quad[4];
  int i;

  for ( i = 0 ; i < 4 ; i++ )
    quad[i] = &leaves[state[i]];

  Quad *q = hashtbl_find(htbl, hash(quad), quad);

  return q->node.n.skip->node.l.map;
}
