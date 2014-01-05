#ifndef BIGINT_H
#define BIGINT_H

struct BigInt;

typedef struct BigInt BigInt;

const BigInt *bi_zero;

int bi_log2(const BigInt *b);
int bi_digit(const BigInt *b, int d);
int bi_iszero(const BigInt *b);

BigInt *bi_copy(const BigInt *b);

BigInt *bi_power_2(int k);
BigInt *bi_plus_int(const BigInt *b, int i);
void bi_add_to(BigInt *a, const BigInt *b);

int bi_to_int(const BigInt *b);
BigInt *bi_from_int(int i);

void bi_free(BigInt *b);

void bi_print(BigInt *b);
void bi_test();

#endif
