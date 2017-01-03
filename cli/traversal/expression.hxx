// file      : cli/traversal/expression.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_TRAVERSAL_EXPRESSION_HXX
#define CLI_TRAVERSAL_EXPRESSION_HXX

#include <cli/traversal/elements.hxx>
#include <cli/semantics/expression.hxx>

namespace traversal
{
  struct expression: node<semantics::expression> {};
}

#endif // CLI_TRAVERSAL_EXPRESSION_HXX
