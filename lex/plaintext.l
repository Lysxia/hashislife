%option prefix="plaintext"
%option noyywrap
%{
#include "darray.h"
#include "runlength.h"

#define YY_DECL struct LifeRle yylex(void)

void plaintext_push(Darray *rle_da, char value, int repeat);
%}
%%
            struct LifeRle rle =
            {
              .tokens = NULL,
              .x = 0,
              .y = 0,
              .r = 0,
            };
            Darray *rle_da = da_new(sizeof(struct RleToken));
            int linum = 0;
            #define PUSH(x, n) \
              struct = plaintext_push(rle_da, &t, (x), (n))
![^\n]*\n   linum++; // Comment lines
\.+         plaintext_push(rle_da,    DEAD_RLE_TOKEN, yyleng);
O+          plaintext_push(rle_da,   ALIVE_RLE_TOKEN, yyleng);
\n+         plaintext_push(rle_da, NEWLINE_RLE_TOKEN, yyleng); linum+=yyleng;
<<EOF>>     {
            plaintext_push(rle_da,     END_RLE_TOKEN, 1);
            rle.tokens = da_unpack(rle_da, NULL);
            return rle;
            }
.|\n        {
            fprintf(stderr, "Line %d: unexpected character.", linum);
            da_destroy(rle_da);
            return rle;
            }
%%
void plaintext_push(Darray *rle_da, char value, int repeat)
{
  struct RleToken *t = da_alloc(rle_da);
  t->value.char_ = value;
  t->repeat = repeat;
}
