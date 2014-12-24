%option prefix="plaintext"
%option noyywrap
%{
#include "darray.h"
#include "runlength.h"

#define YY_DECL struct LifeRle yylex(void)
%}
%s H
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
            #define TOKEN(x) (union Tokenizable) { .char_ = (x) }
            #define PUSH(x, n) \
              BEGIN(0); \
              push_token(rle_da, TOKEN(x), (n))
            BEGIN(H);

<H>![^\n]*\n  line_num++; // Comment lines
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
            fprintf(stderr, "Line %d: unexpected character '%c'\n",
              line_num, yytext[0]);
            da_destroy(rle_da);
            rle.x = rle.y = -1;
            return rle;
            }
%%
