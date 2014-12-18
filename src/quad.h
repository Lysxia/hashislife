#ifndef QUAD_H
#define QUAD_H

typedef struct QuadList QuadList;

typedef struct Quad Quad;

//! List of quad elements, with task identifier
/*! */
struct QuadList {
  int       ql_skip_id;
  Quad     *ql_head;
  QuadList *ql_tail;
};

//! Quadtree internal node
struct QInNode
{
  Quad *skip;           /*!< State of the center area after `2^depth` steps */
  QuadList *short_skip; /*! State of the center area after less than
                          `2^depth` steps */
  Quad *sub[4];
  /*!< Subtrees,
    layout:

        0 1
        2 3
  */
};

//! Quadtree leaf
struct QLeaf
{
  int map[4];
  /*!< 2x2 square map,
    layout:

        0 1
        2 3
  */
};

//! Quadtree leaf or node
union Node
{
  struct QInNode n;
  struct QLeaf l;
};

//! Quadtree node
/*! A quadtree of depth `2^depth+1` represents a square area
  with a side length of `2^(depth+1)` cells.
                                                   
  A leaf has depth 0;
  an internal node has depth `depth > 0`.

  ---

  For an internal node, the state of the center area after exactly
  `2^depth` steps is to be stored at `.node.n.skip`.

  For inferior numbers of steps, the result is to be stored in
  `.node.n.short_skip`.
*/
struct Quad
{
  int         depth;      
  union Node  node;       
  BigInt     *cell_count; /*!< Number of cells */
};

#endif
