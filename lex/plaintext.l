%option prefix="plaintext"
%option noyywrap
%{
#include "darray.h"
#include "runlength.h"

#define YY_DECL int yylex(struct LifeRle *rle)
%}
%s H
%%
            *rle = (struct LifeRle) {
              .tokens = NULL,
              .x = 0,
              .y = 0,
              .r = 0,
            };
            DArray rle_da;
            da_init(&rle_da, sizeof(struct RleToken));
            int line_num = 0;
            int line_len = 0;
            #define TOKEN(x) (union Tokenizable) { .char_ = (x) }
            #define PUSH(x, n) \
              if ( NULL == push_token(&rle_da, TOKEN(x), (n)) ) \
              { \
                da_destroy(&rle_da); \
                return 1; \
              } \
              BEGIN(0)
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
            rle->x += yyleng;
            if ( rle->y < line_len )
              rle->y = line_len;
            line_len = 0;
            }
<<EOF>>     {
            PUSH(END_RLE_TOKEN, 1);
            rle->tokens = da_unpack(&rle_da, NULL);
            if ( 0 < line_len ) rle->x++;
            if ( rle->y < line_len )
              rle->y = line_len;
            return 0;
            }
.|\n        {
            fprintf(stderr, "Line %d: unexpected character '%c'\n",
              line_num, yytext[0]);
            da_destroy(&rle_da);
            rle->x = rle->y = -1;
            return 2;
            }
%%
