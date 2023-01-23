// file      : cli/traversal/doc.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_TRAVERSAL_DOC_HXX
#define CLI_TRAVERSAL_DOC_HXX

#include "elements.hxx"
#include "../semantics/doc.hxx"

namespace traversal
{
  struct doc: node<semantics::doc> {};
}

#endif // CLI_TRAVERSAL_DOC_HXX
