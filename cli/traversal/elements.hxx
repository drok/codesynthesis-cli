// file      : cli/traversal/elements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2018 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_TRAVERSAL_ELEMENTS_HXX
#define CLI_TRAVERSAL_ELEMENTS_HXX

#include <cutl/compiler/traversal.hxx>

#include <cli/semantics/elements.hxx>

namespace traversal
{
  using namespace cutl;

  //
  //
  typedef compiler::dispatcher<semantics::node> node_dispatcher;
  typedef compiler::dispatcher<semantics::edge> edge_dispatcher;

  //
  //
  struct node_base: node_dispatcher, edge_dispatcher
  {
    void
    edge_traverser (edge_dispatcher& d)
    {
      edge_dispatcher::traverser (d);
    }

    edge_dispatcher&
    edge_traverser ()
    {
      return *this;
    }

    using node_dispatcher::dispatch;
    using edge_dispatcher::dispatch;

    using edge_dispatcher::iterate_and_dispatch;
  };

  struct edge_base: edge_dispatcher, node_dispatcher
  {
    void
    node_traverser (node_dispatcher& d)
    {
      node_dispatcher::traverser (d);
    }

    node_dispatcher&
    node_traverser ()
    {
      return *this;
    }

    using edge_dispatcher::dispatch;
    using node_dispatcher::dispatch;

    using node_dispatcher::iterate_and_dispatch;
  };

  inline edge_base&
  operator>> (node_base& n, edge_base& e)
  {
    n.edge_traverser (e);
    return e;
  }

  inline node_base&
  operator>> (edge_base& e, node_base& n)
  {
    e.node_traverser (n);
    return n;
  }

  //
  //
  template <typename X>
  struct node: compiler::traverser_impl<X, semantics::node>,
               virtual node_base
  {
  };

  template <typename X>
  struct edge: compiler::traverser_impl<X, semantics::edge>,
               virtual edge_base
  {
  };

  // Edges
  //

  struct names: edge<semantics::names>
  {
    names ()
    {
    }

    names (node_dispatcher& n)
    {
      node_traverser (n);
    }

    virtual void
    traverse (type&);
  };

  // Nodes
  //

  struct nameable: node<semantics::nameable> {};

  template <typename T>
  struct scope_template: node<T>
  {
  public:
    virtual void
    traverse (T& s)
    {
      names (s);
    }

    virtual void
    names (T& s)
    {
      names (s, *this);
    }

    virtual void
    names (T& s, edge_dispatcher& d)
    {
      this->iterate_and_dispatch (s.names_begin (), s.names_end (), d);
    }
  };

  typedef scope_template<semantics::scope> scope;

  struct type: node<semantics::type> {};
}

#endif // CLI_TRAVERSAL_ELEMENTS_HXX
