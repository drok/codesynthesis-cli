// file      : cli/traversal/unit.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include "unit.hxx"

namespace traversal
{
  // cxx_includes
  //
  void cxx_includes::
  traverse (type& e)
  {
    dispatch (e.includee ());
  }

  // cli_includes
  //
  void cli_includes::
  traverse (type& e)
  {
    dispatch (e.includee ());
  }

  // cli_unit
  //
  void cli_unit::
  traverse (type& u)
  {
    pre (u);
    iterate_and_dispatch (
      u.includes_begin (), u.includes_end (), edge_traverser ());
    names (u);
    post (u);
  }

  void cli_unit::
  pre (type&)
  {
  }

  void cli_unit::
  post (type&)
  {
  }
}
