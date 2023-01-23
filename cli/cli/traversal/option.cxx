// file      : cli/traversal/option.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include "option.hxx"
#include "expression.hxx"

namespace traversal
{
  // belongs
  //
  void belongs::
  traverse (type& e)
  {
    dispatch (e.type ());
  }

  // initialized
  //
  void initialized::
  traverse (type& e)
  {
    dispatch (e.expression ());
  }

  // option
  //
  void option::
  traverse (type& o)
  {
    pre (o);
    belongs (o);
    if (o.initialized_p ())
      initialized (o);
    post (o);
  }

  void option::
  pre (type&)
  {
  }

  void option::
  belongs (type& o)
  {
    belongs (o, edge_traverser ());
  }

  void option::
  initialized (type& o)
  {
    initialized (o, edge_traverser ());
  }

  void option::
  post (type&)
  {
  }
}
