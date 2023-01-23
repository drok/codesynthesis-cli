// file      : cli/semantics/namespace.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_SEMANTICS_NAMESPACE_HXX
#define CLI_SEMANTICS_NAMESPACE_HXX

#include "elements.hxx"

namespace semantics
{
  class namespace_: public scope
  {
  public:
    namespace_ (path const& file, size_t line, size_t column)
        : node (file, line, column)
    {
    }

    namespace_ ()
    {
    }
  };
}

#endif // CLI_SEMANTICS_NAMESPACE_HXX
