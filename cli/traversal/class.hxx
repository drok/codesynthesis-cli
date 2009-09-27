// file      : cli/traversal/class.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_TRAVERSAL_CLASS_HXX
#define CLI_TRAVERSAL_CLASS_HXX

#include <traversal/elements.hxx>
#include <semantics/class.hxx>

namespace traversal
{
  struct class_: scope_template<semantics::class_>
  {
    virtual void
    traverse (type&);

    virtual void
    pre (type&);

    virtual void
    post (type&);
  };
}

#endif // CLI_TRAVERSAL_CLASS_HXX