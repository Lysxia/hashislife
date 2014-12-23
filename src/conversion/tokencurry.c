#include <assert.h>
#include <limits.h>

#include "bitmaps.h"
#include "conversion_aux.h"
#include "hashtbl.h"

union Tokenizable param_leaf_(void *ptr, union Tokenizable cells[4])
{
  (void) ptr;
  unsigned l = 0;
  for ( int i = 0 ; i < 4 ; i++ )
  {
    assert( cells[i].int_ == 0 || cells[i].int_ == 1 );
    l |= cells[i].int_ << (3 - i);
  }
  return (union Tokenizable) { .ptr_ = leaf(l) };
}

const struct ZipParam param_leaf =
{
  .tc = {
    .f = &param_leaf_,
    .args = NULL,
  },
  .deflt = {
    .value = { .int_ = 0 },
    .repeat = INT_MAX,
  },
};

union Tokenizable param_cons_with_(void *args_, union Tokenizable quad_[4])
{
  struct ZipArgs_cons *args = args_;
  Quad *quad[4];
  for ( int k = 0 ; k < 4 ; k++ )
    quad[k] = quad_[k].ptr_;
  return (union Tokenizable)
  {
    .ptr_ = (void *) cons_quad(args->h, quad, args->d)
  };
}

struct ZipParam param_cons_with(Hashtbl *htbl, int d)
{
  struct ZipArgs_cons *args = malloc(sizeof(*args));
  *args = (struct ZipArgs_cons)
  {
    .h = htbl,
    .d = d,
  };
  return (struct ZipParam)
  {
    .tc = (struct TokenCurry)
    {
      .f = &param_cons_with_,
      .args = (void *) args,
    },
    .deflt = (struct RleToken)
    {
      .value = { .ptr_ = dead_space(htbl, d-1) },
      .repeat = INT_MAX,
    },
  };
}

