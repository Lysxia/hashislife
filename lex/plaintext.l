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
            int line_num = 0;
            int line_len = 0;
            #define PUSH(x, n) plaintext_push(rle_da, (x), (n));
![^\n]*\n   line_num++; // Comment lines
\.+         {
            PUSH(DEAD_RLE_TOKEN, yyleng);
            line_len += yyleng;
            }
O+          {
            PUSH(ALIVE_RLE_TOKEN, yyleng);
            line_len += yyleng;
            }
\n+         {
            PUSH(NEWLINE_RLE_TOKEN, yyleng);
            line_num += yyleng;
            rle.x += yyleng;
            if ( rle.y < line_len )
              rle.y = line_len;
            line_len = 0;
            }
<<EOF>>     {
            PUSH(END_RLE_TOKEN, 1);
            rle.tokens = da_unpack(rle_da, NULL);
            if ( 0 < line_len ) rle.x++;
            if ( rle.y < line_len )
              rle.y = line_len;
            return rle;
            }
.|\n        {
            fprintf(stderr, "Line %d: unexpected character.", line_num);
            da_destroy(rle_da);
            rle.x = rle.y = -1;
            return rle;
            }
%%
void plaintext_push(Darray *rle_da, char value, int repeat)
{
  struct RleToken *t = da_alloc(rle_da);
  t->value.char_ = value;
  t->repeat = repeat;
}
