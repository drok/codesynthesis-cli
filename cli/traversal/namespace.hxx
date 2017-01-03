// file      : cli/traversal/namespace.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_TRAVERSAL_NAMESPACE_HXX
#define CLI_TRAVERSAL_NAMESPACE_HXX

#include <cli/traversal/elements.hxx>
#include <cli/semantics/namespace.hxx>

namespace traversal
{
  struct namespace_: scope_template<semantics::namespace_>
  {
    virtual void
    traverse (type&);

    virtual void
    pre (type&);

    virtual void
    post (type&);
  };
}

#endif // CLI_TRAVERSAL_NAMESPACE_HXX
