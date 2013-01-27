/* Hashtable of Quad using a Node as key */

#ifndef HASHTBL_H
#define HASHTBL_H

const unsigned int init_addr_size = 20;
const unsigned int init_size = 1048576;

typedef struct Hashtbl Hashtbl;

/* Internal node or leaf */
typedef union Node Node;

/* List like type */
typedef struct Quad Quad;

struct Hashtbl {
  int size;
  struct Quad** tbl;
};

union Node {
  // internal node
  struct {
    struct Quad *ul,*ur,*bl,*br,*next;
  } n;

  // leaf
  struct {
    int map[4];
    /* 0 1
     * 2 3
     * */
  } l;
};

struct Quad {
  int depth; // head information
  union Node node;
  struct Quad* tl; // tail
};

int hash(Node);

Hashtbl hashtbl_new();

/* Hash is computed before calling find and/or add */
Quad* hashtbl_find(Hashtbl hashtbl, int h, Node key);

void hashtbl_add(Hashtbl hashtbl, int h, Quad* elt);

Quad* list_find(Quad* list, Node key);

void hashtbl_free(Hashtbl);

#endif
