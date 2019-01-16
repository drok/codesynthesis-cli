// file      : cli/traversal/namespace.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <cli/traversal/namespace.hxx>

namespace traversal
{
  void namespace_::
  traverse (type& n)
  {
    pre (n);
    names (n);
    post (n);
  }

  void namespace_::
  pre (type&)
  {
  }

  void namespace_::
  post (type&)
  {
  }
}
