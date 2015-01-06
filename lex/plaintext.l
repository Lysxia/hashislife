%option prefix="plaintext"
%option noyywrap
%{
#include "darray.h"
#include "parsers.h"
#include "runlength.h"

#define YY_DECL int yylex(struct LifeRle *rle)
%}
%s H
%%
            rle->r = 0;
            DArray rle_da;
            da_init(&rle_da, sizeof(struct RleToken));
            size_t comment_lines = 0, line_num = 0;
            size_t line_len = 0, max_line_len = 0;
            BigInt repeat;
            #define TOKEN(x) (union Tokenizable) { .char_ = (x) }
            #define DESTROY_IF(x) \
              if ( x ) \
              { \
                da_destroy(&rle_da); \
                return 1; \
              } // end of DESTROY_IF
            #define PUSH(x, n) \
              bi_simple(&repeat, (n)); \
              DESTROY_IF( NULL == push_token(&rle_da, TOKEN(x), &repeat) ); \
              BEGIN(0) // end of PUSH
            BEGIN(H);

<H>![^\n]*\n  comment_lines++; // Comment lines
\.+/O       {
            PUSH(DEAD_RLE_TOKEN, yyleng);
            line_len += yyleng;
            }
\.+         ; // Ignore trailing dots
O+          {
            PUSH(ALIVE_RLE_TOKEN, yyleng);
            line_len += yyleng;
            }
\n|\r|\r\n  {
            PUSH(NEWLINE_RLE_TOKEN, 1);
            if ( max_line_len < line_len )
              max_line_len = line_len;
            line_len = 0;
            line_num++;
            }
<<EOF>>     {
            DESTROY_IF( life_rle_unpack(rle, &rle_da) );
            if ( 0 < line_len ) line_num++;
            if ( max_line_len < line_len )
              max_line_len = line_len;
            bi_simple(&rle->x, max_line_len);
            bi_simple(&rle->y, line_num);
            return 0;
            }
.|\n        {
            fprintf(stderr, "Line %zu: unexpected character ",
              comment_lines + line_num);
            if ( isprint(yytext[0]) )
              fprintf(stderr, "'%c'\n", yytext[0]);
            else
              fprintf(stderr, "'%d'\n", yytext[0]);
            da_destroy(&rle_da);
            return 2;
            }
%%
