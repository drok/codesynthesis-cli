// file      : cli/traversal/class.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_TRAVERSAL_CLASS_HXX
#define CLI_TRAVERSAL_CLASS_HXX

#include <cli/traversal/elements.hxx>
#include <cli/semantics/class.hxx>

namespace traversal
{
  struct inherits: edge<semantics::inherits>
  {
    inherits () {}
    inherits (node_dispatcher& n) {node_traverser (n);}

    virtual void
    traverse (type&);
  };

  struct class_: scope_template<semantics::class_>
  {
    virtual void
    traverse (type&);

    virtual void
    pre (type&);

    virtual void
    inherits (type&);

    virtual void
    inherits (type&, edge_dispatcher&);

    virtual void
    post (type&);
  };
}

#endif // CLI_TRAVERSAL_CLASS_HXX
