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
  int repeat = 1;
  int line_num = 0;
  *rle = (struct LifeRle) {
    .tokens = NULL,
    .x = 0,
    .y = 0,
    .r = 0,
  };
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
    DESTROY_IF( NULL == push_token(&rle_da, TOKEN(x), repeat) ); \
    repeat = 1; \
    BEGIN(D) // end of PUSH
  #define PARSE_ERROR() da_destroy(&rle_da); rle->x = rle->y = -1; return 2
  BEGIN(H); // Header

<H>#[^\n]*\n  line_num++;
<H>[^\n]*\n {
  char r_buff[100];
  switch ( sscanf(yytext, " x = %d, y = %d, rule = %21s ",
                            &rle->x, &rle->y, r_buff) )
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
  line_num++;
  BEGIN(D);
  }
<D>{DIGIT}+ {
  repeat = atoi(yytext);
  BEGIN(0);
  }
{DEAD}     PUSH(   DEAD_RLE_TOKEN);
{ALIVE}    PUSH(  ALIVE_RLE_TOKEN);
{NEWLINE}  PUSH(NEWLINE_RLE_TOKEN);
<INITIAL>{END}  {
  fprintf(stderr, "Line %d: unexpected character '!' following a count\n",
                        line_num);
  PARSE_ERROR();
  }
{END} {
  repeat = 1;
  PUSH(END_RLE_TOKEN);
  DESTROY_IF( da_unpack(&rle_da, (void **) &rle->tokens, NULL) )
  return 0;
  }
[ \t]*      ;
\n+|\r+     line_num += yyleng;
(\r\n)+     line_num += yyleng/2;
<*>.|\n {
  fprintf(stderr, "Line %d: unexpected character ", line_num);
  if ( isprint(yytext[0]) )
    fprintf(stderr, "'%c'", yytext[0]);
  else
    fprintf(stderr, "'%d'", yytext[0]);
  PARSE_ERROR();
  }
<<EOF>> {
  fputs("End of file encountered before closing token '!'\n", stderr);
  PARSE_ERROR();
  }
%%
