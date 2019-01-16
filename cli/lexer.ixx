// file      : cli/lexer.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

// lexer::xchar
//
inline lexer::xchar::
xchar (int_type v, std::size_t l, std::size_t c)
    : v_ (v), l_ (l), c_ (c)
{
}

inline lexer::xchar::
operator char_type () const
{
  return traits_type::to_char_type (v_);
}

inline lexer::xchar::int_type lexer::xchar::
value () const
{
  return v_;
}

inline std::size_t lexer::xchar::
line () const
{
  return l_;
}

inline std::size_t lexer::xchar::
column () const
{
  return c_;
}

// lexer
//
inline bool lexer::
valid () const
{
  return valid_;
}

inline bool lexer::
is_alpha (char c) const
{
  return std::isalpha (c, loc_);
}

inline bool lexer::
is_oct_digit (char c) const
{
  return std::isdigit (c, loc_) && c != '8' && c != '9';
}

inline bool lexer::
is_dec_digit (char c) const
{
  return std::isdigit (c, loc_);
}

inline bool lexer::
is_hex_digit (char c) const
{
  return std::isxdigit (c, loc_);
}

inline bool lexer::
is_alnum (char c) const
{
  return std::isalnum (c, loc_);
}

inline bool lexer::
is_space (char c) const
{
  return std::isspace (c, loc_);
}

inline bool lexer::
is_eos (xchar const& c) const
{
  return c.value () == xchar::traits_type::eof ();
}

inline char lexer::
to_upper (char c) const
{
  return std::toupper (c, loc_);
}
