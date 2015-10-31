// file      : cli/semantics/doc.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_SEMANTICS_DOC_HXX
#define CLI_SEMANTICS_DOC_HXX

#include <semantics/elements.hxx>

namespace semantics
{
  // Scope-level documentation node.
  //
  class doc: public nameable, public doc_strings
  {
  public:
    doc (path const& file, size_t line, size_t column)
        : node (file, line, column) {}
  };
}

#endif // CLI_SEMANTICS_DOC_HXX