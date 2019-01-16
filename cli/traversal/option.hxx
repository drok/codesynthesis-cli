// file      : cli/traversal/option.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_TRAVERSAL_OPTION_HXX
#define CLI_TRAVERSAL_OPTION_HXX

#include <cli/traversal/elements.hxx>
#include <cli/semantics/option.hxx>

namespace traversal
{
  struct belongs: edge<semantics::belongs>
  {
    belongs ()
    {
    }

    belongs (node_dispatcher& n)
    {
      node_traverser (n);
    }

    virtual void
    traverse (type&);
  };

  struct initialized: edge<semantics::initialized>
  {
    initialized ()
    {
    }

    initialized (node_dispatcher& n)
    {
      node_traverser (n);
    }

    virtual void
    traverse (type&);
  };

  struct option: node<semantics::option>
  {
    virtual void
    traverse (type&);

    virtual void
    pre (type&);

    virtual void
    belongs (type&);

    virtual void
    initialized (type&);

    virtual void
    post (type&);

    void
    belongs (type& o, edge_dispatcher& d)
    {
      d.dispatch (o.belongs ());
    }

    void
    initialized (type& o, edge_dispatcher& d)
    {
      d.dispatch (o.initialized ());
    }
  };
}

#endif // CLI_TRAVERSAL_OPTION_HXX
