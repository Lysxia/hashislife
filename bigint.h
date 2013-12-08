#ifndef BIGINT_H
#define BIGINT_H

typedef struct BigInt BigInt;

struct BigInt {
  int *digits, len;
};

int bi_length(BigInt b);

int bi_digit(BigInt b, int d);

BigInt bi_plus_int(BigInt b, int i);

void bi_free(BigInt b);

void bi_print(BigInt b);

void bi_test();

#endif
