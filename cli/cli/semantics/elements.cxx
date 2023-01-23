// file      : cli/semantics/elements.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include <libcutl/compiler/type-info.hxx>

#include "elements.hxx"

namespace semantics
{
  // nameable
  //
  string nameable::
  fq_name () const
  {
    string const& n (name ());

    if (n.empty ())
      return n;
    else
      return scope ().fq_name () + "::" + n;
  }

  // scope
  //

  scope::names_iterator_pair scope::
  find (string const& name) const
  {
    names_map::const_iterator i (names_map_.find (name));

    if (i == names_map_.end ())
      return names_iterator_pair (names_.end (), names_.end ());
    else
      return names_iterator_pair (i->second.begin (), i->second.end ());
  }

  scope::names_iterator scope::
  find (names& e)
  {
    list_iterator_map::iterator i (iterator_map_.find (&e));
    return i != iterator_map_.end () ? i->second : names_.end ();
  }

  void scope::
  add_edge_left (names& e)
  {
    names_list::iterator it (names_.insert (names_.end (), &e));
    iterator_map_[&e] = it;

    for (names::name_iterator i (e.name_begin ()); i != e.name_end (); ++i)
      names_map_[*i].push_back (&e);
  }

  void scope::
  remove_edge_left (names& e)
  {
    list_iterator_map::iterator i (iterator_map_.find (&e));
    assert (i != iterator_map_.end ());

    names_.erase (i->second);
    iterator_map_.erase (i);

    for (names::name_iterator ni (e.name_begin ()); ni != e.name_end (); ++ni)
    {
      names_map::iterator j (names_map_.find (*ni));

      for (names_list::iterator i (j->second.begin ());
           i != j->second.end (); ++i)
      {
        if (*i == &e)
          i = j->second.erase (i);
      }
    }
  }

  // type info
  //
  namespace
  {
    struct init
    {
      init ()
      {
        using compiler::type_info;

        // node
        //
        insert (type_info (typeid (node)));

        // edge
        //
        insert (type_info (typeid (edge)));

        // names
        //
        {
          type_info ti (typeid (names));
          ti.add_base (typeid (edge));
          insert (ti);
        }

        // nameable
        //
        {
          type_info ti (typeid (nameable));
          ti.add_base (typeid (node));
          insert (ti);
        }

        // scope
        //
        {
          type_info ti (typeid (scope));
          ti.add_base (typeid (nameable));
          insert (ti);
        }

        // type
        //
        {
          type_info ti (typeid (type));
          ti.add_base (typeid (node));
          insert (ti);
        }
      }
    } init_;
  }
}
