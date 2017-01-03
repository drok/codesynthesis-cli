// file      : cli/semantics/namespace.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_SEMANTICS_NAMESPACE_HXX
#define CLI_SEMANTICS_NAMESPACE_HXX

#include <cli/semantics/elements.hxx>

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
