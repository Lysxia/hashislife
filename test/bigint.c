#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "bigint.h"

int main(void) {
  const char *num1 = "123456789ABCDEF0123456789ABCDEF0";
  const char *num2 = "1111111111111111111111111111111";

  BigInt n1, n2, nn, m, m_, one;
  size_t hint;
  bi_from_string(&n1, num1, 16);
  bi_from_string(&n2, num2, 16);
  bi_add(&nn, &n1, &n2);
  bi_simple(&one, 1);
  int gt = bi_compare(&nn, &one, &hint);
  assert( gt > 0 );
  bi_sub(&m, &nn, &one, hint);
  bi_copy(&m_, &m);
  bi_div_int(&m_, 3);
  char s[1000];
  puts("Binary sum (read right to left)");
  bi_binary_string(s, &n1); puts(s);
  bi_binary_string(s, &n2); puts(s);
  bi_binary_string(s, &nn); puts(s);
  puts("Subtract one");
  bi_binary_string(s, &m); puts(s);
  puts("Divide by 3");
  bi_binary_string(s, &m_); puts(s);
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
  puts("Subtract one");
  bi_to_string(s, &m, 16);
  for ( size_t m_len = strlen(s) ; m_len < s1_len + 1 ; m_len++ )
    putchar(' ');
  puts(s);
  return 0;
}

