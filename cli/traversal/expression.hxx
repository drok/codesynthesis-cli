// file      : cli/traversal/expression.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
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
