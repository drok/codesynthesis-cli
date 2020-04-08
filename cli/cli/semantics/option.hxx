// file      : cli/semantics/option.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_SEMANTICS_OPTION_HXX
#define CLI_SEMANTICS_OPTION_HXX

#include <cli/semantics/elements.hxx>

namespace semantics
{
  //
  //
  class option;

  //
  //
  class belongs: public edge
  {
  public:
    typedef semantics::type type_type;
    typedef semantics::option option_type;

    option_type&
    option () const
    {
      return *option_;
    }

    type_type&
    type () const
    {
      return *type_;
    }

  public:
    void
    set_left_node (option_type& n)
    {
      option_ = &n;
    }

    void
    set_right_node (type_type& n)
    {
      type_ = &n;
    }

  private:
    option_type* option_;
    type_type* type_;
  };

  //
  //
  class expression;

  //
  //
  class initialized: public edge
  {
  public:
    typedef semantics::option option_type;
    typedef semantics::expression expression_type;

    option_type&
    option () const
    {
      return *option_;
    }

    expression_type&
    expression () const
    {
      return *expression_;
    }

  public:
    void
    set_left_node (option_type& n)
    {
      option_ = &n;
    }

    void
    set_right_node (expression_type& n)
    {
      expression_ = &n;
    }

  private:
    option_type* option_;
    expression_type* expression_;
  };

  //
  //
  class option: public nameable
  {
  public:
    typedef semantics::belongs belongs_type;
    typedef semantics::type type_type;

    belongs_type&
    belongs () const
    {
      return *belongs_;
    }

    type_type&
    type () const
    {
      return belongs_->type ();
    }

  public:
    typedef semantics::initialized initialized_type;

    bool
    initialized_p () const
    {
      return initialized_ != 0;
    }

    initialized_type&
    initialized () const
    {
      return *initialized_;
    }

    expression&
    initializer () const
    {
      return initialized_->expression ();
    }

  public:
    typedef doc_strings::const_iterator doc_iterator;

    doc_iterator
    doc_begin () const
    {
      return doc_.begin ();
    }

    doc_iterator
    doc_end () const
    {
      return doc_.end ();
    }

    doc_strings const&
    doc () const
    {
      return doc_;
    }

    doc_strings&
    doc ()
    {
      return doc_;
    }

  public:
    option (path const& file, size_t line, size_t column)
        : node (file, line, column), initialized_ (0)
    {
    }

    void
    add_edge_left (belongs_type& e)
    {
      belongs_ = &e;
    }

    void
    add_edge_left (initialized_type& e)
    {
      initialized_ = &e;
    }

  private:
    belongs_type* belongs_;
    initialized_type* initialized_;
    doc_strings doc_;
  };
}

#endif // CLI_SEMANTICS_OPTION_HXX
