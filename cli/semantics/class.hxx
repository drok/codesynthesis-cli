// file      : cli/semantics/class.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_SEMANTICS_CLASS_HXX
#define CLI_SEMANTICS_CLASS_HXX

#include <vector>

#include <cli/semantics/elements.hxx>

namespace semantics
{
  class class_;

  class inherits: public edge
  {
  public:
    class_&
    base () const
    {
      return *base_;
    }

    class_&
    derived () const
    {
      return *derived_;
    }

  public:
    void
    set_left_node (class_& n)
    {
      derived_ = &n;
    }

    void
    set_right_node (class_& n)
    {
      base_ = &n;
    }

  protected:
    class_* base_;
    class_* derived_;
  };

  class class_: public scope
  {
  private:
    typedef std::vector<inherits*> inherits_list;

  public:
    bool
    abstract () const
    {
      return abstract_;
    }

    void
    abstract (bool a)
    {
      abstract_ = a;
    }

  public:
    typedef pointer_iterator<inherits_list::const_iterator> inherits_iterator;

    inherits_iterator
    inherits_begin () const
    {
      return inherits_.begin ();
    }

    inherits_iterator
    inherits_end () const
    {
      return inherits_.end ();
    }

  public:
    class_ (path const& file, size_t line, size_t column)
        : node (file, line, column), abstract_ (false)
    {
    }

    void
    add_edge_left (inherits& e)
    {
      inherits_.push_back (&e);
    }

    void
    add_edge_right (inherits&) {}

    using scope::add_edge_left;
    using scope::add_edge_right;

  private:
    bool abstract_;
    inherits_list inherits_;
  };
}

#endif // CLI_SEMANTICS_CLASS_HXX
