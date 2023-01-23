// file      : cli/traversal/elements.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include "elements.hxx"

namespace traversal
{
  void names::
  traverse (type& e)
  {
    dispatch (e.named ());
  }
}
