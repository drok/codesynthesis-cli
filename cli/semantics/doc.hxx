// file      : cli/semantics/doc.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_SEMANTICS_DOC_HXX
#define CLI_SEMANTICS_DOC_HXX

#include <cli/semantics/elements.hxx>

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
