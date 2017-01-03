// file      : cli/semantics/option.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <cutl/compiler/type-info.hxx>

#include <cli/semantics/option.hxx>

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

        // belongs
        //
        {
          type_info ti (typeid (belongs));
          ti.add_base (typeid (edge));
          insert (ti);
        }

        // initializes
        //
        {
          type_info ti (typeid (initialized));
          ti.add_base (typeid (edge));
          insert (ti);
        }

        // option
        //
        {
          type_info ti (typeid (option));
          ti.add_base (typeid (nameable));
          insert (ti);
        }
      }
    } init_;
  }
}
