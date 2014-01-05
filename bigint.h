#ifndef BIGINT_H
#define BIGINT_H

struct BigInt;

typedef struct BigInt BigInt;

const BigInt bi_zero;

int bi_log2(BigInt b);
int bi_digit(BigInt b, int d);
int bi_iszero(BigInt b);

BigInt bi_copy(BigInt b);

void bi_canonize(BigInt *b);

BigInt bi_power_2(int k);
BigInt bi_plus_int(BigInt b, int i);
BigInt *bi_add_to(BigInt *a, BigInt b);

int bi_to_int(BigInt b);
BigInt bi_from_int(int i);

void bi_free(BigInt b);

void bi_print(BigInt b);
void bi_test();

#endif
