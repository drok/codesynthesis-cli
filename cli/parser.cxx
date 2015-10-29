// file      : cli/parser.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <unistd.h>    // stat
#include <sys/types.h> // stat
#include <sys/stat.h>  // stat

#include <fstream>
#include <sstream>
#include <iostream>

#include "token.hxx"
#include "lexer.hxx"
#include "parser.hxx"

#include "semantics.hxx"

using namespace std;
using namespace semantics;

const char* keywords[] =
{
  "include",
  "namespace",
  "class",
  "signed",
  "unsigned",
  "bool",
  "char",
  "wchar_t",
  "short",
  "int",
  "long",
  "float",
  "double"
};

const char* punctuation[] = {
  ";", ",", ":", "::", "{", "}", /*"(", ")",*/ "=", "|"};

// Output the token type and value in a format suitable for diagnostics.
//
std::ostream&
operator<< (std::ostream& os, token const& t)
{
  switch (t.type ())
  {
  case token::t_eos:
    {
      os << "end-of-stream";
      break;
    }
  case token::t_keyword:
    {
      os << "keyword '" << keywords[t.keyword ()] << "'";
      break;
    }
  case token::t_identifier:
    {
      os << "identifier '" << t.identifier () << "'";
      break;
    }
  case token::t_punctuation:
    {
      os << "'" << punctuation[t.punctuation ()] << "'";
      break;
    }
  case token::t_cxx_path_lit:
    {
      os << "c++ path literal";
      break;
    }
  case token::t_cli_path_lit:
    {
      os << "cli path literal";
      break;
    }
  case token::t_string_lit:
    {
      os << "string literal";
      break;
    }
  case token::t_char_lit:
    {
      os << "char literal";
      break;
    }
  case token::t_bool_lit:
    {
      os << "bool literal";
      break;
    }
  case token::t_int_lit:
    {
      os << "integer literal";
      break;
    }
  case token::t_float_lit:
    {
      os << "floating point literal";
      break;
    }
  case token::t_call_expr:
    {
      os << "call expression";
      break;
    }
  case token::t_template_expr:
    {
      os << "template expression";
      break;
    }
  }

  return os;
}

// RAII-style set new value on construction, restore old one on destruction.
//
template <typename T>
struct auto_restore
{
  auto_restore (T*& var, T* new_val = 0)
      : var_ (var), old_val_ (var_)
  {
    if (new_val != 0)
      var_ = new_val;
  }

  void
  set (T* new_val) {var_ = new_val;}

  ~auto_restore () {var_ = old_val_;}

private:
  T*& var_;
  T* old_val_;
};


void parser::
recover (token& t)
{
  // Recover by skipping past next ';'.
  //
  for (;; t = lexer_->next ())
  {
    if (t.type () == token::t_eos)
      break;

    if (t.punctuation () == token::p_semi)
    {
      t = lexer_->next ();
      break;
    }
  }
}

auto_ptr<cli_unit> parser::
parse (std::istream& is, path const& p)
{
  auto_ptr<cli_unit> unit (new cli_unit (p, 1, 1));

  {
    path ap (p);
    ap.absolute ();
    ap.normalize ();
    include_map_[ap] = unit.get ();
  }

  root_ = cur_ = unit.get ();

  lexer l (is, p.string ());
  lexer_ = &l;

  doc_count_ = 0;

  path_ = &p;
  valid_ = true;

  def_unit ();

  if (!valid_ || !l.valid ())
    throw invalid_input ();

  return unit;
}

void parser::
def_unit ()
{
  token t (lexer_->next ());

  // include-decl-seq
  //
  while (t.keyword () == token::k_include)
  {
    try
    {
      include_decl ();
      t = lexer_->next ();
    }
    catch (error const&)
    {
      valid_ = false;
      recover (t);
    }
  }

  auto_restore<scope> new_scope (scope_, cur_);

  // decl-seq
  //
  while (t.type () != token::t_eos)
  {
    try
    {
      if (decl (t))
      {
        t = lexer_->next ();
        continue;
      }

      cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
           << "expected namespace, class, or documentation declaration or "
           << "instead of " << t
           << endl;
      throw error ();
    }
    catch (error const&)
    {
      valid_ = false;
      break; // Non-recoverable error.
    }
  }
}

void parser::
include_decl ()
{
  token t (lexer_->next ());
  token::token_type tt (t.type ());

  if (tt != token::t_cxx_path_lit && tt != token::t_cli_path_lit)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected path literal instead of " << t << endl;
    throw error ();
  }

  string const& l (t.literal ());
  includes::kind_type ik (l[0] == '<' ? includes::bracket : includes::quote);

  path f;
  try
  {
    f = path (string (l, 1, l.size () - 2));
  }
  catch (const invalid_path& e)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "'" << e.path () << "' is not a valid filesystem path" << endl;
    valid_ = false;
  }

  if (valid_)
  {
    if (tt == token::t_cxx_path_lit)
    {
      cxx_unit& n (
        root_->new_node<cxx_unit> (*path_, t.line (), t.column ()));
      root_->new_edge<cxx_includes> (*cur_, n, ik, f);
    }
    else
    {
      path p;
      // If this is a quote include, then include relative to the current
      // file.
      //
      if (ik == includes::quote)
      {
        p = path_->directory () / f;
        p.normalize ();
      }
      // Otherwise search the include directories (-I).
      //
      else
      {
        struct stat s;
        for (paths::const_iterator i (include_paths_.begin ());
             i != include_paths_.end (); ++i)
        {
          p = *i / f;
          p.normalize ();

          // Check that the file exist without checking for permissions, etc.
          //
          if (stat (p.string ().c_str (), &s) == 0 && S_ISREG (s.st_mode))
            break;

          p.clear ();
        }

        if (p.empty ())
        {
          cerr << *path_ << ':' << t.line () << ':' << t.column () << ": "
               << "error: file '" << f << "' not found in any of the "
               << "include search directories (-I)" << endl;
          valid_ = false;
        }
      }

      if (valid_)
      {
        // Detect and ignore multiple inclusions.
        //
        path ap (p);
        ap.absolute ();
        ap.normalize ();

        include_map::iterator it (include_map_.find (ap));
        if (it == include_map_.end ())
        {
          cli_unit& n (root_->new_node<cli_unit> (p, 1, 1));
          root_->new_edge<cli_includes> (*cur_, n, ik, f);
          include_map_[ap] = &n;

          auto_restore<cli_unit> new_cur (cur_, &n);
          auto_restore<path const> new_path (path_, &p);

          ifstream ifs (p.string ().c_str ());
          if (ifs.is_open ())
          {
            ifs.exceptions (ifstream::failbit | ifstream::badbit);

            try
            {
              lexer l (ifs, p.string ());
              auto_restore<lexer> new_lexer (lexer_, &l);

              def_unit ();

              if (!l.valid ())
                valid_ = false;
            }
            catch (std::ios_base::failure const&)
            {
              cerr << p << ": error: read failure" << endl;
              valid_ = false;
            }
          }
          else
          {
            cerr << p << ": error: unable to open in read mode" << endl;
            valid_ = false;
          }
        }
        else
          root_->new_edge<cli_includes> (*cur_, *it->second, ik, f);
      }
    }
  }

  t = lexer_->next ();

  if (t.punctuation () != token::p_semi)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected ';' instead of " << t << endl;
    throw error ();
  }
}

bool parser::
decl (token& t)
{
  switch (t.type ())
  {
  case token::t_keyword:
    {
      switch (t.keyword ())
      {
      case token::k_namespace:
        {
          namespace_def ();
          return true;
        }
      case token::k_class:
        {
          class_def ();
          return true;
        }
      default:
        break;
      }

      break;
    }
  case token::t_punctuation:
    {
      if (t.punctuation () != token::p_lcbrace)
        break;

      // Fall through.
    }
  case token::t_string_lit:
    {
      scope_doc (t);
      return true;
    }
  default:
    break;
  }

  return false;
}

void parser::
scope_doc (token& t)
{
  size_t l (t.line ()), c (t.column ());

  doc* d (0);

  if (valid_)
  {
    // Use a counter to give scope-level docs unique names. We use a
    // single counter throughout all units/scope because we could be
    // reopening namespaces.
    //
    ostringstream os;
    os << "doc " << doc_count_++;

    d = &root_->new_node<doc> (*path_, l, c);
    root_->new_edge<names> (*scope_, *d, os.str ());
  }

  if (t.type () == token::t_string_lit)
  {
    // string-literal
    //
    if (valid_)
    {
      d->push_back (doc_string (t));
      cerr << d->name () << " '" << d->back () << "'" << endl;
    }
  }
  else
  {
    // doc-string-seq
    //
    assert (t.punctuation () == token::p_lcbrace);

    for (t = lexer_->next ();; t = lexer_->next ())
    {
      if (t.type () != token::t_string_lit)
      {
        cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
             << "expected documentation string instead of " << t << endl;
        throw error ();
      }

      if (valid_)
      {
        d->push_back (doc_string (t));
        cerr << d->name () << " '" << d->back () << "'" << endl;
      }

      t = lexer_->next ();

      if (t.punctuation () != token::p_comma)
        break;
    }

    if (t.punctuation () != token::p_rcbrace)
    {
      cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
           << "expected '}' instead of " << t << endl;
      throw error ();
    }
  }
}

void parser::
namespace_def ()
{
  token t (lexer_->next ());

  if (t.type () != token::t_identifier)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected identifier instead of " << t << endl;
    throw error ();
  }

  auto_restore<scope> new_scope (scope_);

  if (valid_)
  {
    namespace_& n (
      root_->new_node<namespace_> (*path_, t.line (), t.column ()));
    root_->new_edge<names> (*scope_, n, t.identifier ());
    new_scope.set (&n);
  }

  t = lexer_->next ();

  if (t.punctuation () != token::p_lcbrace)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected '{' instead of " << t << endl;
    throw error ();
  }

  // decl-seq
  //
  t = lexer_->next ();

  while (decl (t))
    t = lexer_->next ();

  if (t.punctuation () != token::p_rcbrace)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected namespace, class, or documentation declaration or '}' "
         << "instead of " << t << endl;
    throw error ();
  }
}

void parser::
class_def ()
{
  token t (lexer_->next ());

  if (t.type () != token::t_identifier)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected identifier instead of " << t << endl;
    throw error ();
  }

  class_* n (0);
  if (valid_)
  {
    n = &root_->new_node<class_> (*path_, t.line (), t.column ());
    root_->new_edge<names> (*scope_, *n, t.identifier ());
  }

  t = lexer_->next ();

  // inheritance-spec
  //
  if (t.punctuation () == token::p_colon)
  {
    for (;;)
    {
      t = lexer_->next ();
      size_t line (t.line ()), col (t.column ());

      string name;
      if (!qualified_name (t, name))
      {
        cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
             << "expected qualified name instead of " << t << endl;
        throw error ();
      }

      string ns;

      // If it is a fully-qualifed name, then start from the global namespace.
      // Otherwise, from the current scope.
      //
      if (name[0] == ':')
        name = string (name, 2, string::npos);
      else
        ns = scope_->fq_name ();

      if (class_* b = cur_->lookup<class_> (ns, name))
        root_->new_edge<inherits> (*n, *b);
      else
      {
        cerr << *path_ << ':' << line << ':' << col << ": error: "
             << "unable to resolve base class '" << name << "'" << endl;
        valid_ = false;
      }

      if (t.punctuation () != token::p_comma)
        break;
    }
  }

  // abstract-spec
  //
  if (t.punctuation () == token::p_eq)
  {
    t = lexer_->next ();

    if (t.type () != token::t_int_lit || t.literal () != "0")
    {
      cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
           << "expected '0' instead of " << t << endl;
      throw error ();
    }

    if (n != 0)
      n->abstract (true);

    t = lexer_->next ();
  }

  if (t.punctuation () != token::p_lcbrace)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected '{' instead of " << t << endl;
    throw error ();
  }

  auto_restore<scope> new_scope (scope_, n);

  // decl-seq
  //
  t = lexer_->next ();

  for (;;)
  {
    try
    {
      if (!option_def (t))
        break;

      t = lexer_->next ();
    }
    catch (error const&)
    {
      valid_ = false;
      recover (t);
    }
  }

  if (t.punctuation () != token::p_rcbrace)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected option declaration or '}' instead of " << t << endl;
    throw error ();
  }

  t = lexer_->next ();

  if (t.punctuation () != token::p_semi)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected ';' instead of " << t << endl;
    throw error ();
  }
}

bool parser::
option_def (token& t)
{
  size_t l (t.line ()), c (t.column ());

  // type-spec
  //
  // These two functions set t to the next token if they return
  // true.
  //
  string type_name;

  if (!qualified_name (t, type_name) && !fundamental_type (t, type_name))
    return false;

  option* o (0);

  if (valid_)
  {
    o = &root_->new_node<option> (*path_, l, c);
    type& t (root_->new_type (*path_, l, c, type_name));
    root_->new_edge<belongs> (*o, t);
  }

  // option-name-seq
  //
  names::name_list nl;
  for (;;)
  {
    switch (t.type ())
    {
    case token::t_identifier:
      {
        if (valid_)
          nl.push_back (t.identifier ());

        break;
      }
    case token::t_string_lit:
      {
        if (valid_)
        {
          // Get rid of '"'.
          //
          string r;
          string const& l (t.literal ());
          char p ('\0');

          for (size_t i (0), n (l.size ()); i < n; ++i)
          {
            if (l[i] == '"' && p != '\\')
              continue;

            // We need to keep track of \\ escapings so we don't confuse
            // them with \", as in "\\".
            //
            if (l[i] == '\\' && p == '\\')
              p = '\0';
            else
              p = l[i];

            r += l[i];
          }

          nl.push_back (r);
        }

        break;
      }
    default:
      {
        cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
             << "option name expected instead of " << t << endl;
        throw error ();
      }
    }

    t = lexer_->next ();

    if (t.punctuation () == token::p_or)
      t = lexer_->next ();
    else
      break;
  }

  if (valid_)
    root_->new_edge<names> (*scope_, *o, nl);

  // initializer
  //
  std::string ev;
  expression::expression_type et;

  if (t.punctuation () == token::p_eq)
  {
    // assignment initiaizer
    //
    t = lexer_->next ();

    l = t.line ();
    c = t.column ();

    if (qualified_name (t, ev))
    {
      et = expression::identifier;
    }
    else
    {
      switch (t.type ())
      {
      case token::t_string_lit:
        {
          ev = t.literal ();
          et = expression::string_lit;
          t = lexer_->next ();
          break;
        }
      case token::t_char_lit:
        {
          ev = t.literal ();
          et = expression::char_lit;
          t = lexer_->next ();
          break;
        }
      case token::t_bool_lit:
        {
          ev = t.literal ();
          et = expression::bool_lit;
          t = lexer_->next ();
          break;
        }
      case token::t_int_lit:
        {
          ev = t.literal ();
          et = expression::int_lit;
          t = lexer_->next ();
          break;
        }
      case token::t_float_lit:
        {
          ev = t.literal ();
          et = expression::float_lit;
          t = lexer_->next ();
          break;
        }
      case token::t_call_expr:
        {
          ev = t.expression ();
          et = expression::call_expr;
          t = lexer_->next ();
          break;
        }
      default:
        {
          cerr << *path_ << ':' << t.line () << ':' << t.column ()
               << ": error: expected intializer instead of " << t << endl;
          throw error ();
        }
      }
    }
  }
  else if (t.type () == token::t_call_expr)
  {
    // c-tor initializer
    //
    l = t.line ();
    c = t.column ();

    ev = t.expression ();
    et = expression::call_expr;
    t = lexer_->next ();
  }

  if (valid_ && !ev.empty ())
  {
    expression& e (root_->new_node<expression> (*path_, l, c, et, ev));
    root_->new_edge<initialized> (*o, e);
  }

  if (t.punctuation () == token::p_lcbrace)
  {
    // doc-string-seq
    //
    for (t = lexer_->next ();; t = lexer_->next ())
    {
      if (t.type () != token::t_string_lit)
      {
        cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
             << "expected documentation string instead of " << t << endl;
        throw error ();
      }

      if (valid_)
        o->doc ().push_back (doc_string (t));

      t = lexer_->next ();

      if (t.punctuation () != token::p_comma)
        break;
    }

    if (t.punctuation () != token::p_rcbrace)
    {
      cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
           << "expected '}' instead of " << t << endl;
      throw error ();
    }

    t = lexer_->next ();
  }

  if (t.punctuation () != token::p_semi)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected ';' instead of " << t << endl;
    throw error ();
  }

  return true;
}

string parser::
doc_string (token& t)
{
  // Get rid of '"'.
  //
  string t1, t2;
  string const& l (t.literal ());
  char p ('\0');

  for (size_t i (0), n (l.size ()); i < n; ++i)
  {
    if (l[i] == '"' && p != '\\')
      continue;

    // We need to keep track of \\ escapings so we don't confuse
    // them with \", as in "\\".
    //
    if (l[i] == '\\' && p == '\\')
      p = '\0';
    else
      p = l[i];

    t1 += l[i];
  }

  // Get rid of leading and trailing spaces in each line.
  //
  if (t1.size () != 0)
  {
    bool more (true);
    size_t b (0), e, p;

    while (more)
    {
      p = e = t1.find ('\n', b);

      if (p == string::npos)
      {
        e = t1.size ();
        more = false;
      }

      while (b < e && (t1[b] == 0x20 || t1[b] == 0x0D || t1[b] == 0x09))
        ++b;

      --e;

      while (e > b && (t1[e] == 0x20 || t1[e] == 0x0D || t1[e] == 0x09))
        --e;

      if (b <= e)
        t2.append (t1, b, e - b + 1);

      if (more)
      {
        t2 += '\n';
        b = p + 1;
      }
    }
  }

  // Replace every single newlines with single space and all
  // multiple new lines (paragraph marker) with a single newline.
  //
  t1.clear ();
  for (size_t i (0), n (t2.size ()); i < n; ++i)
  {
    if (t2[i] == '\n')
    {
      size_t j (i);
      for (; i + 1 < n && t2[i + 1] == '\n'; ++i) ;

      if (j != 0 && i + 1 != n) // Strip leading and trailing newlines.
        t1 += i != j ? '\n' : ' ';
    }
    else
      t1 += t2[i];
  }

  return t1;
}


bool parser::
qualified_name (token& t, string& r)
{
  if (t.type () != token::t_identifier && t.punctuation () != token::p_dcolon)
    return false;

  r.clear ();

  if (t.punctuation () == token::p_dcolon)
  {
    r += "::";
    t = lexer_->next ();
  }

  for (;;)
  {
    if (t.type () != token::t_identifier)
    {
      cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
           << "expected identifier after '::'" << endl;
      throw error ();
    }

    r += t.identifier ();
    t = lexer_->next ();

    if (t.type () == token::t_template_expr)
    {
      // Template-id.
      //
      r += t.expression ();
      t = lexer_->next ();
    }

    if (t.punctuation () == token::p_dcolon)
    {
      r += "::";
      t = lexer_->next ();
    }
    else
      break;
  }

  return true;
}

bool parser::
fundamental_type (token& t, string& r)
{
  r.clear ();

  switch (t.keyword ())
  {
  case token::k_signed:
  case token::k_unsigned:
    {
      r = t.keyword () == token::k_signed ? "signed" : "unsigned";
      switch ((t = lexer_->next ()).keyword ())
      {
      case token::k_short:
        {
          r += " short";
          switch ((t = lexer_->next ()).keyword ())
          {
          case token::k_int:
            {
              r += " int";
              t = lexer_->next ();
            }
          default:
            break;
          }
          break;
        }
      case token::k_long:
        {
          r += " long";
          switch ((t = lexer_->next ()).keyword ())
          {
          case token::k_int:
            {
              r += " int";
              t = lexer_->next ();
              break;
            }
          case token::k_long:
            {
              r += " long";
              switch ((t = lexer_->next ()).keyword ())
              {
              case token::k_int:
                {
                  r += " int";
                  t = lexer_->next ();
                }
              default:
                break;
              }
              break;
            }
          default:
            break;
          }
          break;
        }
      case token::k_int:
        {
          r += " int";
          switch ((t = lexer_->next ()).keyword ())
          {
          case token::k_short:
            {
              r += " short";
              t = lexer_->next ();
              break;
            }
          case token::k_long:
            {
              r += " long";
              switch ((t = lexer_->next ()).keyword ())
              {
              case token::k_long:
                {
                  r += " long";
                  t = lexer_->next ();
                }
              default:
                break;
              }
              break;
            }
          default:
            break;
          }
          break;
        }
      case token::k_char:
        {
          r += " char";
          t = lexer_->next ();
          break;
        }
      default:
        break;
      }
      break;
    }
  case token::k_short:
  case token::k_long:
    {
      bool l (t.keyword () == token::k_long);
      r = l ? "long" : "short";

      switch ((t = lexer_->next ()).keyword ())
      {
      case token::k_signed:
      case token::k_unsigned:
        {
          r += t.keyword () == token::k_signed ? " signed" : " unsigned";
          switch ((t = lexer_->next ()).keyword ())
          {
          case token::k_int:
            {
              r += " int";
              t = lexer_->next ();
            }
          default:
            break;
          }
          break;
        }
      case token::k_long:
        {
          r += " long";
          switch ((t = lexer_->next ()).keyword ())
          {
          case token::k_signed:
          case token::k_unsigned:
            {
              r += t.keyword () == token::k_signed ? " signed" : " unsigned";
              switch ((t = lexer_->next ()).keyword ())
              {
              case token::k_int:
                {
                  r += " int";
                  t = lexer_->next ();
                }
              default:
                break;
              }
              break;
            }
          case token::k_int:
            {
              r += " int";
              switch ((t = lexer_->next ()).keyword ())
              {
              case token::k_signed:
                {
                  r += " signed";
                  t = lexer_->next ();
                  break;
                }
              case token::k_unsigned:
                {
                  r += " unsigned";
                  t = lexer_->next ();
                  break;
                }
              default:
                break;
              }
              break;
            }
          default:
            break;
          }
          break;
        }
      case token::k_int:
        {
          r += " int";
          switch ((t = lexer_->next ()).keyword ())
          {
          case token::k_signed:
            {
              r += " signed";
              t = lexer_->next ();
              break;
            }
          case token::k_unsigned:
            {
              r += " unsigned";
              t = lexer_->next ();
              break;
            }
          default:
            break;
          }
          break;
        }
      case token::k_double:
        {
          if (l)
          {
            r += " double";
            t = lexer_->next ();
          }
          break;
        }
      default:
        break;
      }
      break;
    }
  case token::k_int:
    {
      r = "int";
      switch ((t = lexer_->next ()).keyword ())
      {
      case token::k_signed:
      case token::k_unsigned:
        {
          r += t.keyword () == token::k_signed ? " signed" : " unsigned";
          switch ((t = lexer_->next ()).keyword ())
          {
          case token::k_short:
            {
              r += " short";
              t = lexer_->next ();
              break;
            }
          case token::k_long:
            {
              r += " long";
              switch ((t = lexer_->next ()).keyword ())
              {
              case token::k_long:
                {
                  r += " long";
                  t = lexer_->next ();
                }
              default:
                break;
              }
            }
          default:
            break;
          }
          break;
        }
      case token::k_short:
        {
          r += " short";
          switch ((t = lexer_->next ()).keyword ())
          {
          case token::k_signed:
            {
              r += " signed";
              t = lexer_->next ();
              break;
            }
          case token::k_unsigned:
            {
              r += " unsigned";
              t = lexer_->next ();
              break;
            }
          default:
            break;
          }
          break;
        }
      case token::k_long:
        {
          r += " long";
          switch ((t = lexer_->next ()).keyword ())
          {
          case token::k_signed:
            {
              r += " signed";
              t = lexer_->next ();
              break;
            }
          case token::k_unsigned:
            {
              r += " unsigned";
              t = lexer_->next ();
              break;
            }
          case token::k_long:
            {
              r += " long";
              switch ((t = lexer_->next ()).keyword ())
              {
              case token::k_signed:
                {
                  r += " signed";
                  t = lexer_->next ();
                  break;
                }
              case token::k_unsigned:
                {
                  r += " unsigned";
                  t = lexer_->next ();
                  break;
                }
              default:
                break;
              }
              break;
            }
          default:
            break;
          }
          break;
        }
      default:
        break;
      }
      break;
    }
  case token::k_char:
    {
      r = "char";
      switch ((t = lexer_->next ()).keyword ())
      {
      case token::k_signed:
        {
          r += " signed";
          t = lexer_->next ();
          break;
        }
      case token::k_unsigned:
        {
          r += " unsigned";
          t = lexer_->next ();
          break;
        }
      default:
        break;
      }
      break;
    }
  case token::k_bool:
    {
      r = "bool";
      t = lexer_->next ();
      break;
    }
  case token::k_wchar:
    {
      r = "wchar_t";
      t = lexer_->next ();
      break;
    }
  case token::k_float:
    {
      r = "float";
      t = lexer_->next ();
      break;
    }
  case token::k_double:
    {
      r = "double";
      switch ((t = lexer_->next ()).keyword ())
      {
      case token::k_long:
        {
          r += " long";
          t = lexer_->next ();
        }
      default:
        break;
      }
      break;
    }
  default:
    return false;
  }

  return true;
}
