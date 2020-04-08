// file      : cli/token.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_TOKEN_HXX
#define CLI_TOKEN_HXX

#include <string>
#include <cstddef> // std::size_t

class token
{
public:
  enum token_type
  {
    t_eos,
    t_keyword,
    t_identifier,
    t_punctuation,
    t_cxx_path_lit,
    t_cli_path_lit,
    t_string_lit,
    t_char_lit,
    t_bool_lit,
    t_int_lit,
    t_float_lit,
    t_call_expr,    // The so called "call expression", e.g., (2, a).
    t_template_expr // The so called "template expression", e.g., <foo, 3>.
  };

  token_type
  type () const;

  std::size_t
  line () const;

  std::size_t
  column () const;

  // Keyword
  //
public:
  enum keyword_type
  {
    k_source,
    k_include,
    k_namespace,
    k_class,
    k_signed,
    k_unsigned,
    k_bool,
    k_char,
    k_wchar,
    k_short,
    k_int,
    k_long,
    k_float,
    k_double,
    k_invalid
  };

  // Return the keyword id if type is t_keyword and k_invalid otherwise.
  //
  keyword_type
  keyword () const;

  // Identifier
  //
public:
  std::string const&
  identifier () const;

  // Punctuation
  //
public:
  enum punctuation_type
  {
    p_semi,
    p_comma,
    p_colon,
    p_dcolon,
    p_lcbrace,
    p_rcbrace,
    // p_lparen,
    // p_rparen,
    p_eq,
    p_or,
    p_invalid
  };

  // Return the punctuation id if type is t_punctuation and p_invalid
  // otherwise.
  //
  punctuation_type
  punctuation () const;

  // Literals.
  //
public:
  std::string const&
  literal () const;

  // Expressions.
  //
public:
  std::string const&
  expression () const;

  // C-tors.
  //
public:
  // EOS.
  //
  token (std::size_t l, std::size_t c);

  token (keyword_type k, std::size_t l, std::size_t c);
  token (punctuation_type p, std::size_t l, std::size_t c);

  // Identifier, literals, and expressions.
  //
  token (token_type t, std::string const& s, std::size_t l, std::size_t c);

private:
  std::size_t l_;
  std::size_t c_;

  token_type type_;
  keyword_type keyword_;
  punctuation_type punctuation_;
  std::string str_;
};

#include <cli/token.ixx>

#endif // CLI_TOKEN_HXX
