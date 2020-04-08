// file      : cli/semantics/unit.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include <cutl/compiler/type-info.hxx>

#include <cli/semantics/unit.hxx>

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

        // includes
        //
        {
          type_info ti (typeid (includes));
          ti.add_base (typeid (edge));
          insert (ti);
        }

        // cxx_includes
        //
        {
          type_info ti (typeid (cxx_includes));
          ti.add_base (typeid (includes));
          insert (ti);
        }

        // cli_includes
        //
        {
          type_info ti (typeid (cli_includes));
          ti.add_base (typeid (includes));
          insert (ti);
        }

        // cxx_unit
        //
        {
          type_info ti (typeid (cxx_unit));
          ti.add_base (typeid (node));
          insert (ti);
        }

        // cli_unit
        //
        {
          type_info ti (typeid (cli_unit));
          ti.add_base (typeid (namespace_));
          insert (ti);
        }
      }
    } init_;
  }
}
