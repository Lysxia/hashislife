#include <stdio.h>
#include <string.h>  
#include "bigint.h"

int main(void) {
  char *num1 = "123456789ABCDEF0123456789ABCDEF0";
  char *num2 = "1111111111111111111111111111111";

  BigInt n1, n2, nn;
  bi_from_string(&n1, num1, 16);
  bi_from_string(&n2, num2, 16);
  bi_add(&nn, &n1, &n2);
  char s[1000];
  puts("Binary sum (read right to left)");
  bi_binary_string(s, &n1); puts(s);
  bi_binary_string(s, &n2); puts(s);
  bi_binary_string(s, &nn); puts(s);
  puts("Hex sum (read left to right)");
  bi_to_string(s, &n1, 16);
  size_t s1_len = strlen(s);
  putchar(' '); puts(s);
  bi_to_string(s, &n2, 16);
  for ( size_t n2_len = strlen(s) ; n2_len < s1_len + 1 ; n2_len++ )
    putchar(' ');
  puts(s);
  bi_to_string(s, &nn, 16);
  for ( size_t nn_len = strlen(s) ; nn_len < s1_len + 1 ; nn_len++ )
    putchar(' ');
  puts(s);
  return 0;
}

