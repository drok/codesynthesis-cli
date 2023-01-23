// file      : cli/semantics/expression.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include <libcutl/compiler/type-info.hxx>

#include "expression.hxx"

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
