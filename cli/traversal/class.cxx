// file      : cli/traversal/class.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include <cli/traversal/class.hxx>

namespace traversal
{
  // inherits
  //
  void inherits::
  traverse (type& i)
  {
    dispatch (i.base ());
  }

  // class_
  //
  void class_::
  traverse (type& c)
  {
    pre (c);
    inherits (c);
    names (c);
    post (c);
  }

  void class_::
  inherits (type& c)
  {
    inherits (c, *this);
  }

  void class_::
  inherits (type& c, edge_dispatcher& d)
  {
    iterate_and_dispatch (c.inherits_begin (), c.inherits_end (), d);
  }

  void class_::
  pre (type&)
  {
  }

  void class_::
  post (type&)
  {
  }
}
