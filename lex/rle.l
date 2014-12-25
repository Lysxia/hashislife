%option prefix="life_rle_"
%option noyywrap
%{
#include "darray.h"
#include "runlength.h"

#define YY_DECL struct LifeRle yylex(void)
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
  struct LifeRle rle =
  {
    .tokens = NULL,
    .x = 0,
    .y = 0,
    .r = 0,
  };
  DArray rle_da;
  da_init(&rle_da, sizeof(struct RleToken));
  #define TOKEN(x) (union Tokenizable) { .char_ = (x) }
  #define PUSH(x) \
    push_token(&rle_da, TOKEN(x), repeat); \
    repeat = 1; \
    BEGIN(D)
  #define RLE_ERROR() da_destroy(&rle_da); rle.x = rle.y = -1; return rle
  BEGIN(H); // Header

<H>#[^\n]*\n  line_num++;
<H>[^\n]*\n {
  char r_buff[100];
  switch ( sscanf(yytext, " x = %d, y = %d, rule = %21s ",
                            &rle.x, &rle.y, r_buff) )
  {
    case 2:
      rle.r = 0;
      break;
    case 3:
      if ( (rle.r = parse_rule(r_buff)) != (rule) (-1) )
        break;
    case EOF:
    case 1:
    default:
      perror("Incorrect header");
      RLE_ERROR();
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
  RLE_ERROR();
  }
{END} {
  repeat = 1;
  PUSH(END_RLE_TOKEN);
  rle.tokens = da_unpack(&rle_da, NULL);
  return rle;
  }
[ \t]*      ;
\n          line_num++;
<*>.|\n {
  fprintf(stderr, "Line %d: unexpected character '%c'\n",
                        line_num,                 yytext[0]);
  RLE_ERROR();
  }
<<EOF>> {
  perror("End of file encountered before closing token '!'");
  RLE_ERROR();
  }
%%
