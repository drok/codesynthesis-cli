// file      : cli/semantics/elements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_SEMANTICS_ELEMENTS_HXX
#define CLI_SEMANTICS_ELEMENTS_HXX

#include <map>
#include <list>
#include <vector>
#include <string>
#include <cstddef> // std::size_t
#include <cstdlib> // std::abort
#include <utility> // std::pair
#include <cassert>

#include <cutl/fs/path.hxx>

#include <cutl/container/graph.hxx>
#include <cutl/container/pointer-iterator.hxx>

#include <cutl/compiler/context.hxx>

namespace semantics
{
  using namespace cutl;

  using std::size_t;
  using std::string;

  using container::graph;
  using container::pointer_iterator;

  using compiler::context;


  //
  //
  using fs::path;
  using fs::invalid_path;

  //
  //
  typedef std::vector<string> doc_strings;

  //
  //
  class node;
  class edge;


  //
  //
  class edge
  {
  public:
    virtual
    ~edge () {}

  public:
    typedef semantics::context context_type;

    context_type&
    context ()
    {
      return context_;
    }

  public:
    template <typename X>
    bool
    is_a () const
    {
      return dynamic_cast<X const*> (this) != 0;
    }

  private:
    context_type context_;
  };

  //
  //
  class node
  {
  public:
    virtual
    ~node () {}

  public:
    typedef semantics::context context_type;

    context_type&
    context ()
    {
      return context_;
    }

  public:
    path const&
    file () const
    {
      return file_;
    }

    size_t
    line () const
    {
      return line_;
    }

    size_t
    column () const
    {
      return column_;
    }

  public:
    template <typename X>
    bool
    is_a () const
    {
      return dynamic_cast<X const*> (this) != 0;
    }

  public:
    node (path const& file, size_t line, size_t column)
        : file_ (file), line_ (line), column_ (column)
    {
    }

    // For virtual inheritance. Should never be actually called.
    //
    node ()
        : file_ ("")
    {
      std::abort ();
    }

    // Sink functions that allow extensions in the form of one-way
    // edges.
    //
    void
    add_edge_right (edge&)
    {
    }

  private:
    context_type context_;
    path file_;
    size_t line_;
    size_t column_;
  };

  //
  //
  class scope;
  class nameable;


  //
  //
  class names: public edge
  {
  public:
    typedef semantics::scope scope_type;

    typedef std::vector<string> name_list;
    typedef name_list::const_iterator name_iterator;

    // First name.
    //
    string const&
    name () const
    {
      return names_[0];
    }

    name_iterator
    name_begin () const
    {
      return names_.begin ();
    }

    name_iterator
    name_end () const
    {
      return names_.end ();
    }

    scope_type&
    scope () const
    {
      return *scope_;
    }

    nameable&
    named () const
    {
      return *named_;
    }

  public:
    names (string const& name)
    {
      names_.push_back (name);
    }

    names (name_list const& names)
        : names_ (names)
    {
    }

    void
    set_left_node (scope_type& n)
    {
      scope_ = &n;
    }

    void
    set_right_node (nameable& n)
    {
      named_ = &n;
    }

    void
    clear_left_node (scope_type& n)
    {
      assert (scope_ == &n);
      scope_ = 0;
    }

    void
    clear_right_node (nameable& n)
    {
      assert (named_ == &n);
      named_ = 0;
    }

  protected:
    scope_type* scope_;
    nameable* named_;
    name_list names_;
  };


  //
  //
  class nameable: public virtual node
  {
  public:
    typedef semantics::scope scope_type;

    string
    name () const
    {
      return named_->name ();
    }

    string
    fq_name () const;

    scope_type&
    scope ()
    {
      return named_->scope ();
    }

    scope_type const&
    scope () const
    {
      return named_->scope ();
    }

    names&
    named ()
    {
      return *named_;
    }

  public:
    nameable ()
        : named_ (0)
    {
    }

    void
    add_edge_right (names& e)
    {
      assert (named_ == 0);
      named_ = &e;
    }

    void
    remove_edge_right (names& e)
    {
      assert (named_ == &e);
      named_ = 0;
    }

    using node::add_edge_right;

  private:
    names* named_;
  };


  //
  //
  class scope: public virtual nameable
  {
  protected:
    typedef std::list<names*> names_list;
    typedef std::map<names*, names_list::iterator> list_iterator_map;
    typedef std::map<string, names_list> names_map;

  public:
    typedef pointer_iterator<names_list::iterator> names_iterator;
    typedef pointer_iterator<names_list::const_iterator> names_const_iterator;

    typedef
    std::pair<names_const_iterator, names_const_iterator>
    names_iterator_pair;

  public:
    names_iterator
    names_begin ()
    {
      return names_.begin ();
    }

    names_iterator
    names_end ()
    {
      return names_.end ();
    }

    names_const_iterator
    names_begin () const
    {
      return names_.begin ();
    }

    names_const_iterator
    names_end () const
    {
      return names_.end ();
    }

    bool
    names_empty () const
    {
      return names_.empty ();
    }

    virtual names_iterator_pair
    find (string const& name) const;

    names_iterator
    find (names&);

  public:
    scope (path const& file, size_t line, size_t column)
        : node (file, line, column)
    {
    }

    scope ()
    {
    }

    void
    add_edge_left (names&);

    void
    remove_edge_left (names&);

    using nameable::add_edge_right;

  private:
    names_list names_;
    list_iterator_map iterator_map_;
    names_map names_map_;
  };

  //
  //
  class type: public node
  {
  public:
    string const&
    name () const
    {
      return name_;
    }

  public:
    type (path const& file, size_t line, size_t column, string const& name)
        : node (file, line, column), name_ (name)
    {
    }

  private:
    string name_;
  };
}

#endif // CLI_SEMANTICS_ELEMENTS_HXX
