// file      : cli/semantics/expression.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2018 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <cutl/compiler/type-info.hxx>

#include <cli/semantics/expression.hxx>

namespace semantics
{
  // type info
  //
  namespace
  {
    struct init
    {
      init ()
      {
        using compiler::type_info;

        type_info ti (typeid (expression));
        ti.add_base (typeid (node));
        insert (ti);
      }
    } init_;
  }
}
