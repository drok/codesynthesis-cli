// file      : cli/token.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2018 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

inline token::token_type token::
type () const
{
  return type_;
}

inline std::size_t token::
line () const
{
  return l_;
}

inline std::size_t token::
column () const
{
  return c_;
}

inline token::keyword_type token::
keyword () const
{
  return type_ == t_keyword ? keyword_ : k_invalid;
}

inline std::string const& token::
identifier () const
{
  return str_;
}

inline token::punctuation_type token::
punctuation () const
{
  return type_ == t_punctuation ? punctuation_ : p_invalid;
}

inline std::string const& token::
literal () const
{
  return str_;
}

inline std::string const& token::
expression () const
{
  return str_;
}

inline token::
token (std::size_t l, std::size_t c)
    : l_ (l), c_ (c), type_ (t_eos)
{
}

inline token::
token (keyword_type k, std::size_t l, std::size_t c)
    : l_ (l), c_ (c), type_ (t_keyword), keyword_ (k)
{
}

inline token::
token (punctuation_type p, std::size_t l, std::size_t c)
    : l_ (l), c_ (c), type_ (t_punctuation), punctuation_ (p)
{
}

inline token::
token (token_type t, std::string const& s, std::size_t l, std::size_t c)
    : l_ (l), c_ (c), type_ (t), str_ (s)
{
}
