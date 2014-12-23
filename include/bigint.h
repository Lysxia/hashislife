#ifndef BIGINT_H
#define BIGINT_H

struct BigInt;

typedef struct BigInt BigInt;

BigInt *bi_zero(void);

const BigInt *bi_zero_const;

int bi_log2(const BigInt *b);
int bi_slice(const BigInt *b, int c); // get 31 bits starting from the c-th
int bi_digit(const BigInt *b, int d);
int bi_iszero(const BigInt *b);

BigInt *bi_copy(const BigInt *b);

BigInt *bi_power_2(int k);
BigInt *bi_plus_int(const BigInt *b, int i);
BigInt *bi_minus_pow(const BigInt *b, int e, int *neg);
BigInt *bi_add(const BigInt *a, const BigInt *b);

int     bi_to_int(const BigInt *b);
BigInt *bi_from_int(int i);

// Ignores isolated commas
// "10,00", base=10 -> 1000 (0b111010000)
// "10,,00", base=10 -> 10 (0b1010)
BigInt *bi_from_string(const char *c, int base);

void bi_free(BigInt *b);

void bi_print(const BigInt *b);
void bi_test();

char** bi_to_char_mat(
  const BigInt ***bm,
  int m,
  int n,
  int height);

#endif
