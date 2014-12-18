#ifndef QUAD_H
#define QUAD_H

typedef struct Quad_map Quad_map;

typedef struct Quad Quad;

struct InNode
{
  Quad     *sub[4]; // subtrees : 0:upper left,  1:upper right,
  Quad_map *next;   //            2:bottom left, 3:bottom right
};

struct Leaf
{
  int map[4];
  // 0 1
  // 2 3
};

union Node
{
  struct InNode n;
  struct Leaf l;
};

struct Quad
{
  int         depth;      // quad tree for a square map with side 2^(depth+1)
  BigInt     *cell_count;
  union Node  node;
};

#endif
