// file      : cli/lexer.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_LEXER_HXX
#define CLI_LEXER_HXX

#include <map>
#include <string>
#include <locale>
#include <cstddef> // std::size_t
#include <istream>

#include <cli/token.hxx>

class lexer
{
public:
  lexer (std::istream& is, std::string const& id);

  token
  next ();

  bool
  valid () const;

protected:
  class xchar
  {
  public:
    typedef std::char_traits<char> traits_type;
    typedef traits_type::int_type int_type;
    typedef traits_type::char_type char_type;

    xchar (int_type v, std::size_t l, std::size_t c);

    operator char_type () const;

    int_type
    value () const;

    std::size_t
    line () const;

    std::size_t
    column () const;

  private:
    int_type v_;
    std::size_t l_;
    std::size_t c_;
  };

  xchar
  peek ();

  xchar
  get ();

  void
  unget (xchar);

protected:
  class invalid_input {};

  void
  skip_spaces ();

  token
  identifier (xchar);

  token
  int_literal (xchar,
               bool neg = false,
               std::size_t ml = 0,
               std::size_t mc = 0);

  token
  char_literal (xchar);

  token
  string_literal (xchar);

  std::string
  string_literal_trailer ();

  token
  path_literal (xchar);

  token
  call_expression (xchar);

  token
  template_expression (xchar);

protected:
  bool
  is_alpha (char c) const;

  bool
  is_oct_digit (char c) const;

  bool
  is_dec_digit (char c) const;

  bool
  is_hex_digit (char c) const;

  bool
  is_alnum (char c) const;

  bool
  is_space (char c) const;

  bool
  is_eos (xchar const& c) const;

  char
  to_upper (char c) const;

private:
  typedef std::map<std::string, token::keyword_type> keyword_map;

  std::locale loc_;
  std::istream& is_;
  std::string id_;
  std::size_t l_;
  std::size_t c_;

  keyword_map keyword_map_;

  bool eos_;
  bool include_; // Literal in include or source.
  bool valid_;

  xchar buf_;
  bool unget_;
};

#include <cli/lexer.ixx>

#endif // CLI_LEXER_HXX
