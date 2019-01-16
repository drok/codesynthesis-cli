// file      : cli/semantics/expression.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_SEMANTICS_EXPRESSION_HXX
#define CLI_SEMANTICS_EXPRESSION_HXX

#include <string>
#include <cli/semantics/elements.hxx>

namespace semantics
{
  //
  //
  class initialized;

  //
  //
  class expression: public node
  {
  public:
    enum expression_type
    {
      string_lit,
      char_lit,
      bool_lit,
      int_lit,
      float_lit,
      call_expr,
      identifier
    };

    expression_type
    type () const
    {
      return type_;
    }

    std::string const&
    value () const
    {
      return value_;
    }

  public:
    initialized&
    initializes () const
    {
      return *initializes_;
    }

  public:
    expression (path const& file,
                size_t line,
                size_t column,
                expression_type type,
                std::string const& value)
        : node (file, line, column), type_ (type), value_ (value)
    {
    }

    void
    add_edge_right (initialized& e)
    {
      initializes_ = &e;
    }

  private:
    initialized* initializes_;

    expression_type type_;
    std::string value_;
  };
}

#endif // CLI_SEMANTICS_EXPRESSION_HXX
