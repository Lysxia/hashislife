#ifndef BIGINT_H
#define BIGINT_H

typedef struct BigInt BigInt;

struct BigInt {
  int *digits, len;
};

const BigInt bi_zero;

int bi_length(BigInt b);
int bi_digit(BigInt b, int d);
int bi_iszero(BigInt b);

BigInt bi_copy(BigInt b);
int bi_flip(BigInt b, int d);

BigInt *bi_canonize(BigInt *b);

BigInt bi_power_2(int k);
BigInt bi_plus_int(BigInt b, int i);

int bi_to_int(BigInt b);

void bi_free(BigInt b);

void bi_print(BigInt b);
void bi_test();

#endif
