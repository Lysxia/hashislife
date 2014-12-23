%option prefix="plaintext"
%option noyywrap
%{
#include "darray.h"
#include "runlength.h"

#define YY_DECL struct LifeRle yylex(void)

void plaintext_push(Darray *rle_da, struct RleToken *t, char value, int repeat);
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
            struct RleToken t =
            {
              .value = { .char_ = END_RLE_TOKEN },
              .repeat = 0,
            };
            int linum = 0;
            #define PUSH(x, n) plaintext_push(rle_da, &t, (x), (n))
![^\n]*\n   linum++; // Comment lines
\.+         PUSH(   DEAD_RLE_TOKEN, yyleng);
O+          PUSH(  ALIVE_RLE_TOKEN, yyleng);
\n+         PUSH(NEWLINE_RLE_TOKEN, yyleng); linum+=yyleng;
<<EOF>>     {
            PUSH(    END_RLE_TOKEN, 1);
            da_push(rle_da, &t);
            rle.tokens = da_unpack(rle_da, NULL);
            return rle;
            }
.|\n        {
            fprintf(stderr, "Line %d: unexpected character.", linum);
            da_destroy(rle_da);
            return rle;
            }
%%
void plaintext_push(Darray *rle_da, struct RleToken *t, char value, int repeat)
{
  if ( value == t->value.char_ )
  {
    t->repeat += repeat;
  }
  else
  {
    if ( t->repeat > 0 )
      da_push(rle_da, t);
    *t = (struct RleToken) { .value = { .char_ = value }, .repeat = repeat };
  }
}
