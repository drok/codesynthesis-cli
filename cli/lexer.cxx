// file      : cli/lexer.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <iostream>

#include "lexer.hxx"

using namespace std;

lexer::
lexer (istream& is, string const& id)
    : loc_ ("C"),
      is_ (is),
      id_ (id),
      l_ (1),
      c_(1),
      eos_ (false),
      include_ (false),
      valid_ (true),
      buf_ (0, 0, 0),
      unget_ (false)
{
  keyword_map_["source"]    = token::k_source;
  keyword_map_["include"]   = token::k_include;
  keyword_map_["namespace"] = token::k_namespace;
  keyword_map_["class"]     = token::k_class;
  keyword_map_["signed"]    = token::k_signed;
  keyword_map_["unsigned"]  = token::k_unsigned;
  keyword_map_["bool"]      = token::k_bool;
  keyword_map_["char"]      = token::k_char;
  keyword_map_["wchar_t"]   = token::k_wchar;
  keyword_map_["short"]     = token::k_short;
  keyword_map_["int"]       = token::k_int;
  keyword_map_["long"]      = token::k_long;
  keyword_map_["float"]     = token::k_float;
  keyword_map_["double"]    = token::k_double;
}

lexer::xchar lexer::
peek ()
{
  if (unget_)
    return buf_;
  else
  {
    if (eos_)
      return xchar (xchar::traits_type::eof (), l_, c_);
    else
    {
      xchar::int_type i (is_.peek ());

      if (i == xchar::traits_type::eof ())
        eos_ = true;

      return xchar (i, l_, c_);
    }
  }
}

lexer::xchar lexer::
get ()
{
  if (unget_)
  {
    unget_ = false;
    return buf_;
  }
  else
  {
    // When is_.get () returns eof, the failbit is also set (stupid,
    // isn't?) which may trigger an exception. To work around this
    // we will call peek() first and only call get() if it is not
    // eof. But we can only call peek() on eof once; any subsequent
    // calls will spoil the failbit (even more stupid).
    //
    xchar c (peek ());

    if (!is_eos (c))
    {
      is_.get ();

      if (c == '\n')
      {
        l_++;
        c_ = 1;
      }
      else
        c_++;
    }

    return c;
  }
}

void lexer::
unget (xchar c)
{
  // Because iostream::unget cannot work once eos is reached,
  // we have to provide our own implementation.
  //
  buf_ = c;
  unget_ = true;
}

token lexer::
next ()
{
  while (true) // Recovery loop.
  {
    bool include (include_);
    include_ = false;

    skip_spaces ();

    xchar c (get ());

    if (is_eos (c))
      return token (c.line (), c.column ());

    try
    {
      switch (c)
      {
      case '\'':
        {
          return char_literal (c);
        }
      case '\"':
        {
          if (include)
            return path_literal (c);
          else
            return string_literal (c);
        }
      case '<':
        {
          if (include)
            return path_literal (c);
          else
            return template_expression (c);
        }
      case ';':
        {
          return token (token::p_semi, c.line (), c.column ());
        }
      case ',':
        {
          return token (token::p_comma, c.line (), c.column ());
        }
      case ':':
        {
          if (peek () == ':')
          {
            get ();
            return token (token::p_dcolon, c.line (), c.column ());
          }

          return token (token::p_colon, c.line (), c.column ());
        }
      case '{':
        {
          return token (token::p_lcbrace, c.line (), c.column ());
        }
      case '}':
        {
          return token (token::p_rcbrace, c.line (), c.column ());
        }
      case '(':
        {
          return call_expression (c);
        }
      case '=':
        {
          return token (token::p_eq, c.line (), c.column ());
        }
      case '|':
        {
          return token (token::p_or, c.line (), c.column ());
        }
      case '-':
        {
          // This can be a beginning of an identifier or a an integer
          // literal. Figure out which one it is.
          //
          xchar p (peek ());

          if (is_dec_digit (p))
            return int_literal (get (), true, c.line (), c.column ());
          else if (is_space (p))
          {
            skip_spaces ();
            p = peek ();

            if (is_dec_digit (p))
              return int_literal (get (), true, c.line (), c.column ());

            // Stray '-'.
            //
            cerr << id_ << ':' << c.line () << ':' << c.column ()
                 << ": error: unexpected character '-'" << endl;
            throw invalid_input ();
          }

          break;
        }
      }

      if (is_alpha (c) || c == '_' || c == '-' || c == '/')
      {
        return identifier (c);
      }

      if (is_dec_digit (c))
      {
        return int_literal (c);
      }

      cerr << id_ << ':' << c.line () << ':' << c.column ()
           << ": error: unexpected character '" << c << "'" << endl;
      throw invalid_input ();
    }
    catch (invalid_input const&)
    {
      valid_ = false;
    }

    // Try to recover.
    //
    do
    {
      c = get ();

      if (is_eos (c))
        return token (c.line (), c.column ());
    } while (c != ';');
  }
}

void lexer::
skip_spaces ()
{
  for (xchar c (peek ());; c = peek ())
  {
    if (is_eos (c))
      break;

    if (c == '/')
    {
      c = get ();
      xchar p (peek ());

      if (p == '/')
      {
        get ();

        // C++ comment. Read until newline or eos.
        //
        for (c = get (); !is_eos (c) && c != '\n'; c = get ()) ;
        continue;
      }
      else if (p == '*')
      {
        get ();

        // C comment.
        //
        for (c = get ();; c = get ())
        {
          if (is_eos (c))
          {
            cerr << id_ << ':' << c.line () << ':' << c.column ()
                 << ": error: end of stream reached while reading "
                 << "C-style comment" << endl;
            throw invalid_input ();
          }

          if (c == '*')
          {
            c = peek ();
            if (c == '/')
            {
              get ();
              break;
            }
          }
        }
        continue;
      }
      else
      {
        unget (c);
        break;
      }
    }

    if (!is_space (c))
      break;

    get ();
  }
}

token lexer::
identifier (xchar c)
{
  size_t ln (c.line ()), cl (c.column ());
  string lexeme;
  lexeme += c;

  bool check (c == '-' || c == '/');

  for (c = peek ();
       !is_eos (c) && (is_alnum (c) || c == '_' || c == '-');
       c = peek ())
  {
    get ();
    lexeme += c;
  }

  // Check for invalid identifiers.
  //
  if (check)
  {
    size_t i (1);

    for (; i < lexeme.size (); ++i)
      if (is_alnum (lexeme[i]) || lexeme[i] == '_')
        break;

    if (i == lexeme.size ())
    {
      cerr << id_ << ':' << c.line () << ':' << c.column () << ": error: "
           << "invalid character sequence '" << lexeme << "'" << endl;
      throw invalid_input ();
    }
  }

  keyword_map::const_iterator i (keyword_map_.find (lexeme));

  if (i != keyword_map_.end ())
  {
    if (i->second == token::k_include || i->second == token::k_source)
      include_ = true;

    return token (i->second, ln, cl);
  }

  if (lexeme == "true" || lexeme == "false")
    return token (token::t_bool_lit, lexeme, ln, cl);

  return token (token::t_identifier, lexeme, ln, cl);
}

token lexer::
int_literal (xchar c, bool neg, size_t ml, size_t mc)
{
  size_t ln (neg ? ml : c.line ()), cl (neg ? mc : c.column ());
  string lexeme;

  if (neg)
    lexeme += '-';

  lexeme += c;

  for (c = peek (); !is_eos (c) && is_dec_digit (c); c = peek ())
  {
    get ();
    lexeme += c;
  }

  return token (token::t_int_lit, lexeme, ln, cl);
}

token lexer::
char_literal (xchar c)
{
  size_t ln (c.line ()), cl (c.column ());
  string lexeme;
  lexeme += c;

  char p (c);

  while (true)
  {
    c = get ();

    if (is_eos (c))
    {
      cerr << id_ << ':' << c.line () << ':' << c.column () << ": error: "
           << "end of stream reached while reading character literal" << endl;
      throw invalid_input ();
    }

    lexeme += c;

    if (c == '\'' && p != '\\')
      break;

    // We need to keep track of \\ escapings so we don't confuse
    // them with \', as in '\\'.
    //
    if (c == '\\' && p == '\\')
      p = '\0';
    else
      p = c;
  }

  return token (token::t_char_lit, lexeme, ln, cl);
}

token lexer::
string_literal (xchar c)
{
  size_t ln (c.line ()), cl (c.column ());
  string lexeme;
  lexeme += c;

  while (true)
  {
    lexeme += string_literal_trailer ();

    // Check if there are more strings.
    //
    skip_spaces ();

    c = peek ();

    if (is_eos (c) || c != '"')
      break;

    get ();
    lexeme += "\"";
  }

  return token (token::t_string_lit, lexeme, ln, cl);
}

string lexer::
string_literal_trailer ()
{
  string r;
  char p ('\0');

  while (true)
  {
    xchar c = get ();

    if (is_eos (c))
    {
      cerr << id_ << ':' << c.line () << ':' << c.column () << ": error: "
           << "end of stream reached while reading string literal" << endl;
      throw invalid_input ();
    }

    r += c;

    if (c == '"' && p != '\\')
      break;

    // We need to keep track of \\ escapings so we don't confuse
    // them with \", as in "\\".
    //
    if (c == '\\' && p == '\\')
      p = '\0';
    else
      p = c;
  }

  return r;
}

token lexer::
path_literal (xchar c)
{
  size_t ln (c.line ()), cl (c.column ());
  string lexeme;
  lexeme += c;

  char end (c == '<' ? '>' : '"');

  while (true)
  {
    c = get ();

    if (is_eos (c))
    {
      cerr << id_ << ':' << c.line () << ':' << c.column () << ": error: "
           << "end of stream reached while reading path literal" << endl;
      throw invalid_input ();
    }

    lexeme += c;

    if (c == end)
      break;
  }

  token::token_type tt;

  if (lexeme.compare (1, 4, "c++:") == 0)
  {
    tt = token::t_cxx_path_lit;
    lexeme = lexeme[0] + string (lexeme, 5, string::npos);
  }
  else if (lexeme.compare (1, 4, "cli:") == 0)
  {
    tt = token::t_cli_path_lit;
    lexeme = lexeme[0] + string (lexeme, 5, string::npos);
  }
  else
  {
    // See if the path ends with .cli. If not, then we assume this is
    // a C++ inclusion.
    //
    size_t n (lexeme.size ());

    if (n > 5 && lexeme.compare (n - 5, 4, ".cli") == 0)
      tt = token::t_cli_path_lit;
    else
      tt = token::t_cxx_path_lit;
  }

  return token (tt, lexeme, ln, cl);
}

token lexer::
call_expression (xchar c)
{
  size_t ln (c.line ()), cl (c.column ());
  string lexeme;
  lexeme += c;
  size_t balance (1);

  while (balance != 0)
  {
    c = get ();

    if (is_eos (c))
    {
      cerr << id_ << ':' << c.line () << ':' << c.column () << ": error: "
           << "end of stream reached while reading call expression" << endl;
      throw invalid_input ();
    }

    lexeme += c;

    switch (c)
    {
    case '(':
      {
        balance++;
        break;
      }
    case ')':
      {
        balance--;
        break;
      }
    }
  }

  return token (token::t_call_expr, lexeme, ln, cl);
}

token lexer::
template_expression (xchar c)
{
  size_t ln (c.line ()), cl (c.column ());
  string lexeme;
  lexeme += c;
  size_t balance (1);

  while (balance != 0)
  {
    c = get ();

    if (is_eos (c))
    {
      cerr << id_ << ':' << c.line () << ':' << c.column () << ": error: "
           << "end of stream reached while reading template expression"
           << endl;
      throw invalid_input ();
    }

    lexeme += c;

    switch (c)
    {
    case '<':
      {
        balance++;
        break;
      }
    case '>':
      {
        balance--;
        break;
      }
    }
  }

  return token (token::t_template_expr, lexeme, ln, cl);
}
