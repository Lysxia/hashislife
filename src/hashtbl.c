#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "hashtbl.h"
#include "hashtbl_aux.h"

/* The address of a leaf is a 4 digit binary number 0123
  representing the 4 bit map
  0 1
  2 3 */
Quad      *leaves = NULL;

/*! Allocates, initializes and returns a new `Hashtbl` structure.

  @param r The initialization precomputes patterns for this rule set.
               (So the same hashtable can not be used for different
               rule sets.)

  \see rule
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
  htbl->rule      = r;
  htbl->size      = init_size;
  htbl->count     = 0;
  htbl->tbl       = malloc(init_size * sizeof(QuadList*));

  htbl->dead_size = init_dead_size;
  htbl->dead_quad = malloc(init_dead_size * sizeof(Quad*));

  htbl->chunks    = chunks_new(sizeof(QuadList), chunk_max_len);

  if (    NULL == htbl->chunks.blocks
       || NULL == htbl->tbl
       || NULL == htbl->dead_quad )
  {
    perror("hashtbl_new(): Failed to allocate one or several field(s).");
    exit(1);
  }

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
  chunks_delete(htbl->chunks);
  free(htbl->tbl);
  free(htbl->dead_quad);
  free(htbl);
}

/*! Leaves are indexed from 0 to `leaves_count`-1 = 15. */
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

/*! This is automatically called at the first call of `hashtbl_new()` */
void hashlife_init(void)
{
  // Initialize quadtree leaves
  leaves = malloc(leaves_count * sizeof(Quad));

  if ( leaves == NULL )
  {
    perror("hashlife_init()");
    exit(1);
  }

  int i;
  for ( i = 0 ; i < leaves_count ; i++ )
  {
    Quad *q = &leaves[i];

    q->depth = 0;
    q->alive = NULL;

    int j;
    for ( j = 0 ; j < 4 ; j++ )
      q->node.l.map[j] = (i >> (3 - j)) & 1;
  }
#ifdef DEBUG_HTBL_NEW
  fprintf(stderr, "init OK\n");
#endif

}

/*! Part of `hashlife_init()` logic.

  Depth 1 nodes and their `.skip` field (`QInNode`; one step progress)
  are computed at hashtable creation (`hashtbl_new()`) */
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
  ql->head.alive = NULL;

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

/*** Memory management ***/

/*! Returns a fresh pointer to a `QuadList` element. */
QuadList *quadlist_alloc(Hashtbl *htbl)
{
  QuadList *ql = chunks_alloc(htbl->chunks);
  if ( NULL == ql )
  {
    perror("quadlist_alloc(): Failed to allocate element.");
    exit(1);
  }
  htbl->count++;
  return ql;
}

/*** Hashtable functions ***/

void binary(uintptr_t w)
{
  int k;
  for ( k = 0 ; k < 32 ; k++, w >>= 1 )
    fprintf(stderr, "%d", (char) (w & 1));
  fprintf(stderr, " !\n");
}

/*! Hashes the pointers to the four subtrees. */
int hash(Quad* key[4])
{
  uintptr_t *a = (uintptr_t *) key, x;

  x = (a[0] >> 6) ^ a[1] ^ (a[2] << 10) ^ (a[3] << 15);

#ifdef HASHSAMPLE
  //sample
  static int t = 0;

  t++;

  if ( 0 == t % 20021 )
  {
    binary(a[0]);
    binary(a[1]);
    binary(a[2]);
    binary(a[3]);
  }
#endif
  return (int) ((unsigned) x % (unsigned) init_size);
}

/*! Find a quadtree in a hashtable.

  \param h `hash(key)`

  Returns a pointer to an already hashed `Quad`,
  or `NULL` if it doesn't exist. */
Quad *hashtbl_find(Hashtbl *htbl, int h, Quad* key[4])
{
  return list_find(htbl->tbl[h], key);
}

/*! Insert element in a hashtable. */
void hashtbl_add(Hashtbl *htbl, int h, QuadList *elt)
{
  elt->tail = htbl->tbl[h];
  htbl->tbl[h] = elt;
}

/*! Find a quadtree in a list. Returns `NULL` if not found.*/
Quad *list_find(QuadList *list, Quad* key[4])
{
  for ( ; NULL != list ; list = list->tail )
  {
    int i;
    for ( i = 0 ; i < 4 ; i++ )
      if ( key[i] != list->head.node.n.sub[i] )
        break;
    if ( i == 4 )
      return &list->head;
  }
  return NULL;
}

int list_length(QuadList *list)
{
  if ( NULL == list )
    return 0;
  else
    return 1 + list_length(list->tail);
}

/*! Clean up (part of `hashtbl_delete()`) */
void quad_free(Quad *q)
{
  if ( NULL != q->alive )
    bi_free(q->alive);
  // Other references (including the `.short_skip` list and `q` itself)
  // are currently managed elsewhere...
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

#define BUCKET_COUNT 100

/*! Print hashtable bucket length distribution. */
void hashtbl_stat(Hashtbl *htbl)
{
  int i, max[BUCKET_COUNT] = {0};
  for ( i = 0 ; i < init_size ; i++ )
  {
    int l = list_length(htbl->tbl[i]);
    max[l >= BUCKET_COUNT ? BUCKET_COUNT - 1 : l]++;
  }

  fprintf(stderr, "length: %d\n", htbl->count);
  for ( i = 0 ; i < BUCKET_COUNT ; i++ )
  {
    if ( max[i] )
      fprintf(stderr, "%3d %9d\n", i, max[i]);
  }

  return;
}

#undef BUCKET_COUNT

/*! Use hashtable to compute the `.skip` field of a depth 1 node,
  given with four leaves given by their indices.
  Return the center leaf by its index (whose 4 least significant bits
  actually give its state). */
int depth1_skip(Hashtbl *htbl, int state[4])
{
  Quad *quad[4];

  int i;
  for ( i = 0 ; i < 4 ; i++ )
    quad[i] = &leaves[state[i]];

  Quad *q = hashtbl_find(htbl, hash(quad), quad);

  return q->node.n.skip - leaves;
}

