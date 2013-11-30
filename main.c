#include <stdio.h>
#include <stdlib.h>
#include "hashlife.h"

int main()
{
  int rule[16] = {0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0};
  hashlife_init(rule);
  hash_info();

  return 0;
}

void test()
{
  int state[4], i;
  while (1)
  {
    for (i=0 ; i<4 ; i++)
    {
      scanf(" %d", state+i);
      if (state[i] < 0)
        return;
    }
    const int *map = step(state);
    for (i=0 ; i<4 ; i++)
      printf("%d ",map[i]);
    printf("\n");
  }
}
