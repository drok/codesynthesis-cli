// file      : cli/traversal/unit.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_TRAVERSAL_UNIT_HXX
#define CLI_TRAVERSAL_UNIT_HXX

#include <cli/traversal/elements.hxx>
#include <cli/semantics/unit.hxx>

namespace traversal
{
  struct cxx_includes: edge<semantics::cxx_includes>
  {
    cxx_includes ()
    {
    }

    cxx_includes (node_dispatcher& n)
    {
      node_traverser (n);
    }

    virtual void
    traverse (type&);
  };

  struct cli_includes: edge<semantics::cli_includes>
  {
    cli_includes ()
    {
    }

    cli_includes (node_dispatcher& n)
    {
      node_traverser (n);
    }

    virtual void
    traverse (type&);
  };

  struct cxx_unit: node<semantics::cxx_unit> {};

  struct cli_unit: scope_template<semantics::cli_unit>
  {
    virtual void
    traverse (type&);

    virtual void
    pre (type&);

    virtual void
    post (type&);
  };
}

#endif // CLI_TRAVERSAL_UNIT_HXX
