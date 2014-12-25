#include <stdio.h>
#include <stdlib.h>

#include "darray.h"

/* Example usage of DArray
  Read numbers from stdin until a nondigit nonwhitespace character is found
  or EOF and print the list of numbers.
*/
int main(void) {
  DArray da_int;
  da_init(&da_int, sizeof(int));
  int num_read;
  while (1) {
    int x;
    int scan = scanf("%d", &x);
    if ( 0 == scan || EOF == scan )
      break;
    if ( NULL == da_push(&da_int, &x) ) {
      da_destroy(&da_int);
      perror("DArray push");
      return 1;
    }
  }
  int *a;
  size_t n;
  if ( da_unpack(&da_int, (void **) &a, &n) ) {
    da_destroy(&da_int);
    perror("DArray unpack");
    return 1;
  }
  for ( size_t i = 0 ; i < n ; i++ )
    printf("%d ", a[i]);
  printf("\n");
  return 0;
}
