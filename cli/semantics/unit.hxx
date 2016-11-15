// file      : cli/semantics/unit.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_SEMANTICS_UNIT_HXX
#define CLI_SEMANTICS_UNIT_HXX

#include <map>
#include <vector>
#include <string>

#include <cli/semantics/elements.hxx>
#include <cli/semantics/namespace.hxx>

namespace semantics
{
  //
  //
  class cli_unit;
  class cxx_unit;

  //
  //
  class includes: public edge
  {
  public:
    enum kind_type {quote, bracket};

    cli_unit&
    includer () const
    {
      return *includer_;
    }

    kind_type
    kind () const
    {
      return kind_;
    }

    path const&
    file () const
    {
      return file_;
    }

  public:
    includes (kind_type kind, path const& file)
        : kind_ (kind), file_ (file)
    {
    }

    void
    set_left_node (cli_unit& n)
    {
      includer_ = &n;
    }

  protected:
    kind_type kind_;
    path file_;
    cli_unit* includer_;
  };

  //
  //
  class cli_includes: public includes
  {
  public:
    cli_unit&
    includee () const
    {
      return *includee_;
    }

  public:
    cli_includes (kind_type kind, path const& file)
        : includes (kind, file)
    {
    }

    void
    set_right_node (cli_unit& n)
    {
      includee_ = &n;
    }

  private:
    cli_unit* includee_;
  };

  //
  //
  class cxx_includes: public includes
  {
  public:
    cxx_unit&
    includee () const
    {
      return *includee_;
    }

  public:
    cxx_includes (kind_type kind, path const& file)
        : includes (kind, file)
    {
    }

    void
    set_right_node (cxx_unit& n)
    {
      includee_ = &n;
    }

  private:
    cxx_unit* includee_;
  };

  //
  //
  class cxx_unit: public node
  {
  public:
    cxx_unit (path const& file, size_t line, size_t column)
        : node (file, line, column)
    {
    }

    void
    add_edge_right (cxx_includes&)
    {
    }
  };

  //
  //
  class cli_unit: public graph<node, edge>, public namespace_
  {
    typedef std::vector<includes*> includes_list;

  public:
    // Lookup a name in the specified starting scope. Empty scope denotes
    // the global namespace. Starting scope should be a fully-qualified
    // name while name can be qualified but should not be fully-qualified
    // (to lookup a fully-qualified name use the global namespace as the
    // starting scope).
    //
    // The lookup starts in this unit and continues in all the units that
    // this unit includes, transitively.
    //
    // The outer flag specifies whether to search the outer scopes.
    //
    template <typename T>
    T*
    lookup (std::string const& scope,
            std::string const& name,
            bool outer = true);

  public:
    typedef
    pointer_iterator<includes_list::const_iterator>
    includes_iterator;

    includes_iterator
    includes_begin () const
    {
      return includes_.begin ();
    }

    includes_iterator
    includes_end () const
    {
      return includes_.end ();
    }

  public:
    cli_unit (path const& file, size_t line, size_t column)
        : node (file, line, column), graph_ (*this)
    {
      // Use a special edge to get this->name() return the global
      // namespace name ("").
      //
      new_edge<global_names> (*this, *this);
    }

  public:
    template <typename T>
    T&
    new_node (path const& file, size_t line, size_t column)
    {
      return graph_.new_node<T> (file, line, column);
    }

    template <typename T, typename A0>
    T&
    new_node (path const& file, size_t line, size_t column, A0 const& a0)
    {
      return graph_.new_node<T> (file, line, column, a0);
    }

    template <typename T, typename A0, typename A1>
    T&
    new_node (path const& file, size_t line, size_t column,
              A0 const& a0, A1 const& a1)
    {
      return graph_.new_node<T> (file, line, column, a0, a1);
    }

    template <typename T, typename A0, typename A1, typename A2>
    T&
    new_node (path const& file, size_t line, size_t column,
              A0 const& a0, A1 const& a1, A2 const& a2)
    {
      return graph_.new_node<T> (file, line, column, a0, a1, a2);
    }

    template <typename T, typename A0, typename A1, typename A2, typename A3>
    T&
    new_node (path const& file, size_t line, size_t column,
              A0 const& a0, A1 const& a1, A2 const& a2, A3 const& a3)
    {
      return graph_.new_node<T> (file, line, column, a0, a1, a2, a3);
    }

  public:
    type&
    new_type (path const& file, size_t l, size_t c, string const& name)
    {
      type_map::iterator i (types_.find (name));

      return i != types_.end ()
        ? *i->second
        : *(types_[name] = &new_node<type> (file, l, c, name));
    }

  protected:
    // Special names edge for the global namespace.
    //
    class global_names: public names
    {
    public:
      global_names ()
          : names ("")
      {
        scope_ = 0;
      }

      void
      set_left_node (cli_unit&)
      {
      }

      void
      set_right_node (nameable& n)
      {
        named_ = &n;
      }

      void
      clear_left_node (cli_unit&)
      {
      }

      void
      clear_right_node (nameable& n)
      {
        assert (named_ == &n);
        named_ = 0;
      }
    };

  public:
    void
    add_edge_left (cli_includes& e)
    {
      includes_.push_back (&e);
    }

    void
    add_edge_left (cxx_includes& e)
    {
      includes_.push_back (&e);
    }

    void
    add_edge_left (global_names&)
    {
    }

    void
    add_edge_right (cli_includes&)
    {
    }

    using namespace_::add_edge_left;
    using namespace_::add_edge_right;

  private:
    typedef std::map<string, type*> type_map;

  private:
    graph<node, edge>& graph_;
    includes_list includes_;
    type_map types_;
  };
}

#include <cli/semantics/unit.txx>

#endif // CLI_SEMANTICS_UNIT_HXX
