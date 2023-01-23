// file      : cli/traversal/unit.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_TRAVERSAL_UNIT_HXX
#define CLI_TRAVERSAL_UNIT_HXX

#include "elements.hxx"
#include "../semantics/unit.hxx"

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
