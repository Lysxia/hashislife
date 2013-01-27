/* Hashtable of Quad using a Node as key */

#ifndef HASHTBL_H
#define HASHTBL_H

const unsigned int init_addr_size = 20;
const unsigned int init_size = 1048576;

typedef struct Hashtbl Hashtbl;

/* Internal node or leaf */
typedef union Node Node;

union Node {
  // internal node
  struct {
    struct Quad *ul,*ur,*bl,*br,*next;
  } n;

  // leaf
  struct {
    int ul_,ur_,bl_,br_;
  } l;
};

/* List like type */
typedef struct Quad Quad;

struct Quad {
  int depth; // head information
  Node node;
  Quad* tl; // tail
};

int hash(Node);

Hashtbl hashtbl_new();

/* Hash is computed before calling find and/or add */
Quad* hashtbl_find(Node key, Hashtbl hashtbl, int h);

void hashtbl_add(Quad* elt, Hashtbl hashtbl, int h);

Quad* list_find(Node key, Quad* list);

void hashtbl_free(Hashtbl);

#endif
