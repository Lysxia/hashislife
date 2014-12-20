#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "hashtbl.h"

int main(void) {
  Hashtbl *htbl = hashtbl_new(CONWAY);
  hashtbl_stat(htbl);
  return 0;
}
