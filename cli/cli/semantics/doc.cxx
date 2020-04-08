// file      : cli/semantics/doc.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include <cutl/compiler/type-info.hxx>

#include <cli/semantics/doc.hxx>

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

        type_info ti (typeid (doc));
        ti.add_base (typeid (nameable));
        insert (ti);
      }
    } init_;
  }
}
