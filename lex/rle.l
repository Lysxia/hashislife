%option prefix="life_rle_"
%option noyywrap
%{
#include <ctype.h>

#include "darray.h"
#include "parsers.h"
#include "runlength.h"

#define YY_DECL int yylex(struct LifeRle *rle)
%}
DIGIT     [0-9]
ALIVE     o
DEAD      b
END       !
NEWLINE   \$
/* Header */
%x H
/* Possibly expecting a repeat count */
%s D
%%
  BigInt repeat;
  bi_simple(&repeat, 1);
  size_t line_num = 0;
  rle->r = 0;
  DArray rle_da;
  da_init(&rle_da, sizeof(struct RleToken));
  #define TOKEN(x) (union Tokenizable) { .char_ = (x) }
  #define DESTROY_IF(x) \
    if ( x ) \
    { \
      da_destroy(&rle_da); \
      return 1; \
    } // end of DESTROY_IF
  #define PUSH(x) \
    DESTROY_IF( NULL == push_token(&rle_da, TOKEN(x), &repeat) ); \
    bi_simple(&repeat, 1); \
    BEGIN(D) // end of PUSH
  #define PARSE_ERROR() da_destroy(&rle_da); return 2
  BEGIN(H); // Header

<H>#[^\n]*\n  line_num++;
<H>[^\n]*\n {
  char x_buff[yyleng], y_buff[yyleng], r_buff[100];
  switch ( sscanf(yytext,
                  " x = %[-0123456789], y = %[-0123456789], rule = %21s ",
                  x_buff, y_buff, r_buff) )
  {
    case 2:
      rle->r = 0;
      break;
    case 3:
      if ( (rle->r = parse_rule(r_buff)) != (rule) (-1) )
        break;
    case EOF:
    case 1:
    default:
      fputs("Incorrect header\n", stderr);
      PARSE_ERROR();
  }
  if ( bi_from_string(&rle->x, x_buff, 10)
    || bi_from_string(&rle->y, y_buff, 10) )
  {
    PARSE_ERROR();
  }
  line_num++;
  BEGIN(D);
  }
<D>{DIGIT}+ {
  bi_from_string(&repeat, yytext, 10);
  BEGIN(0);
  }
{DEAD}     PUSH(   DEAD_RLE_TOKEN);
{ALIVE}    PUSH(  ALIVE_RLE_TOKEN);
{NEWLINE}  PUSH(NEWLINE_RLE_TOKEN);
<INITIAL>{END}  {
  fprintf(stderr, "Line %zu: unexpected character '!' following a count\n",
                        line_num);
  PARSE_ERROR();
  }
{END} {
  DESTROY_IF( life_rle_unpack(rle, &rle_da) );
  return 0;
  }
[ \t]*      ;
\n|\r|\r\n line_num++;
<*>.|\n {
  fprintf(stderr, "Line %zu: unexpected character ", line_num);
  if ( isprint(yytext[0]) )
    fprintf(stderr, "'%c'\n", yytext[0]);
  else
    fprintf(stderr, "'%d'\n", yytext[0]);
  PARSE_ERROR();
  }
<<EOF>> {
  fputs("End of file encountered before closing token '!'\n", stderr);
  PARSE_ERROR();
  }
%%
