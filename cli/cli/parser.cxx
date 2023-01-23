// file      : cli/parser.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef _WIN32
#  include <unistd.h>    // stat
#  include <sys/types.h> // stat
#  include <sys/stat.h>  // stat
#else
#  include <sys/types.h> // _stat
#  include <sys/stat.h>  // _stat(), S_I*

#  ifdef _MSC_VER // Unlikely to be fixed in newer versions.
#    define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#  endif
#endif

#include <fstream>
#include <sstream>
#include <utility>  // move()
#include <iostream>

#include "token.hxx"
#include "lexer.hxx"
#include "parser.hxx"

#include "semantics.hxx"

using namespace std;
using namespace semantics;

// Check that the file exist without checking for permissions, etc.
//
inline static bool
file_exists (const path& p)
{
#ifndef _WIN32
  struct stat s;
  int r (stat (p.string ().c_str (), &s));
#else
  struct _stat s;
  int r (_stat (p.string ().c_str (), &s));
#endif

  return r == 0 && S_ISREG (s.st_mode);
}

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
static std::ostream&
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
  // Recover by skipping past next ';' or '}'.
  //
  for (;; t = lexer_->next ())
  {
    if (t.type () == token::t_eos)
      break;

    token::punctuation_type p (t.punctuation ());

    if (p == token::p_semi || p == token::p_rcbrace)
    {
      t = lexer_->next ();
      break;
    }
  }
}

parser::parse_result parser::
parse (std::istream& is, path const& p)
{
  unique_ptr<cli_unit> unit (new cli_unit (p, 1, 1));

  {
    path ap (p);
    ap.complete ();
    ap.normalize ();
    include_map_[move (ap)] = unit.get ();

    if (collect_dependencies_)
      dependencies_.push_back (path (p).normalize ());
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

  return parse_result {move (unit), move (dependencies_)};
}

void parser::
def_unit ()
{
  token t (lexer_->next ());

  // include-decl-seq
  //
  for (token::keyword_type k (t.keyword ());
       k == token::k_include || k == token::k_source;
       k = t.keyword ())
  {
    try
    {
      if (k == token::k_include)
        include_decl ();
      else
        source_decl ();

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
      if (t.keyword () == token::k_source)
      {
        try
        {
          source_decl ();
          t = lexer_->next ();
        }
        catch (error const&)
        {
          valid_ = false;
          recover (t);
        }

        continue;
      }

      if (decl (t))
      {
        t = lexer_->next ();
        continue;
      }

      cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
           << "expected namespace, class, or documentation instead of "
           << t << endl;
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
source_decl ()
{
  token t (lexer_->next ());

  if (t.type () != token::t_cli_path_lit)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected cli path literal instead of " << t << endl;
    throw error ();
  }

  string const& l (t.literal ());
  bool q (l[0] == '"'); // Quote or braket include?

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
    path p;

    // If this is a quote include, then include relative to the current
    // file.
    //
    if (q)
    {
      p = path_->directory () / f;
      p.normalize ();
    }
    // Otherwise search the include directories (-I).
    //
    else
    {
      for (paths::const_iterator i (include_paths_.begin ());
           i != include_paths_.end (); ++i)
      {
        p = *i / f;
        p.normalize ();

        if (file_exists (p))
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
      if (collect_dependencies_)
      {
        path ap (p);
        ap.complete ();
        ap.normalize ();

        if (include_map_.emplace (move (ap), nullptr).second)
          dependencies_.push_back (p);
      }

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
  }

  t = lexer_->next ();

  if (t.punctuation () != token::p_semi)
  {
    cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
         << "expected ';' instead of " << t << endl;
    throw error ();
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
        for (paths::const_iterator i (include_paths_.begin ());
             i != include_paths_.end (); ++i)
        {
          p = *i / f;
          p.normalize ();

          if (file_exists (p))
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
        ap.complete ();
        ap.normalize ();

        include_map::iterator it (include_map_.find (ap));
        if (it == include_map_.end () || it->second == nullptr)
        {
          if (collect_dependencies_ && it == include_map_.end ())
            dependencies_.push_back (p);

          cli_unit& n (root_->new_node<cli_unit> (p, 1, 1));
          root_->new_edge<cli_includes> (*cur_, n, ik, f);
          include_map_[move (ap)] = &n;

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
    }
    // Fall through.
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
  size_t ln (t.line ()), cl (t.column ());

  // Use a counter to give scope-level docs unique names. We use a
  // single counter throughout all units/scope because we could be
  // reopening namespaces.
  //
  if (t.type () == token::t_string_lit)
  {
    // string-literal
    //
    if (valid_)
    {
      // Enter each ""-enclosed string as a separate documentation
      // entry, handle documentation variables.
      //
      const string& l (t.literal ());

      char p ('\0');
      for (size_t b (0), e (1); e < l.size (); ++e)
      {
        if (l[e] == '"' && p != '\\')
        {
          string s (doc_string (l.c_str () + b, e - b + 1));

          if (!s.empty ())
          {
            doc& d (root_->new_node<doc> (*path_, ln, cl));

            // See if this is a variable assignment: "\<var>=<val>".
            //
            size_t p (0); // '=' position.
            if (s.size () >= 3 && s[0] == '\\' && s[1] != '\\')
            {
              for (p = 1; p != s.size (); ++p)
              {
                char c (s[p]);

                // Variable name should be a C identifier.
                //
                if (!(c == '_' ||
                      ('a' <= c && c <= 'z') ||
                      ('A' <= c && c <= 'Z') ||
                      (p != 1 && '0' <= c && c <= '9')))
                  break;
              }

              if (p == s.size () || s[p] != '=' || p == 1) // Not a variable.
                p = 0;
            }

            if (p != 0)
            {
              root_->new_edge<names> (
                *scope_, d, "var: " + string (s, 1, p - 1));
              s = string (s, p + 1);
            }
            else
            {
              ostringstream os;
              os << "doc: " << doc_count_++;
              root_->new_edge<names> (*scope_, d, os.str ());
            }

            d.push_back (s); // move().
          }

          // If we have more, then make b point to the opening '"'. Second
          // ++e in for() above will make e point to the character after it.
          //
          b = ++e;
          continue;
        }

        // We need to keep track of \\ escapings so we don't confuse
        // them with \", as in \\".
        //
        if (l[e] == '\\' && p == '\\')
          p = '\0';
        else
          p = l[e];
      }
    }
  }
  else
  {
    // doc-string-seq
    //
    assert (t.punctuation () == token::p_lcbrace);

    doc* d (0);
    if (valid_)
    {
      ostringstream os;
      os << "doc: " << doc_count_++;

      d = &root_->new_node<doc> (*path_, ln, cl);
      root_->new_edge<names> (*scope_, *d, os.str ());
    }

    for (t = lexer_->next ();; t = lexer_->next ())
    {
      if (t.type () != token::t_string_lit)
      {
        cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
             << "expected documentation string instead of " << t << endl;
        throw error ();
      }

      if (valid_)
        d->push_back (doc_string (t.literal ().c_str (),
                                  t.literal ().size ()));

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
         << "expected namespace, class, documentation, or '}' instead of "
         << t << endl;
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

  // class-decl-seq
  //
  t = lexer_->next ();

  for (;;)
  {
    try
    {
      if (t.type () == token::t_string_lit ||
          t.punctuation () == token::p_lcbrace)
      {
        scope_doc (t);
        t = lexer_->next ();
      }
      else
      {
        if (!option_def (t))
          break;
      }
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
         << "expected option, documentation, or '}' instead of " << t << endl;
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

  // option-def-trailer
  //
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
        o->doc ().push_back (doc_string (t.literal ().c_str (),
                                         t.literal ().size ()));

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

    // Allow semicolon after option-doc for backwards compatibility.
    //
    if (t.punctuation () == token::p_semi)
      t = lexer_->next ();
  }
  else
  {
    if (t.punctuation () != token::p_semi)
    {
      cerr << *path_ << ':' << t.line () << ':' << t.column () << ": error: "
           << "expected ';' instead of " << t << endl;
      throw error ();
    }

    t = lexer_->next ();
  }

  return true;
}

string parser::
doc_string (const char* l, size_t n)
{
  // Pass 1: get rid of " (as in "foo""bar"), convert \" to just ".
  //
  string t1, t2, t3;
  char p ('\0'); // Previous character.

  for (size_t i (0); i < n; ++i)
  {
    char c (l[i]);

    if (c == '"')
    {
      if (p == '\\')
      {
        t1[t1.size () - 1] = '"'; // Replace \ with ".
        p = c;
      }
      continue;
    }

    // We need to keep track of \\ escapings so we don't confuse them with \",
    // as in \\".
    //
    if (c == '\\' && p == '\\')
      p = '\0';
    else
      p = c;

    t1 += c;
  }

  // Pass two: get rid of leading and trailing spaces in each line. Also
  // handle pre-formatted fragments.
  //
  if (t1.size () != 0)
  {
    bool more (true);
    size_t b (0), e, p;

    bool pre (false);
    size_t m (0); // Number of leading spaces to remove in pre.

    while (more)
    {
      p = e = t1.find ('\n', b);

      if (p == string::npos)
      {
        e = t1.size ();
        more = false;
      }

      if (b != e) // Unless this is just a single newline.
      {
        // In the pre mode we only remove up to m leading whitespaces.
        //
        {
          size_t i (0);
          while (b < e &&
                 (t1[b] == 0x20 || t1[b] == 0x0D || t1[b] == 0x09) &&
                 (!pre || i != m))
          {
            ++b;
            ++i;
          }

          if (!pre)
            m = i;
        }

        --e;
        while (e > b && (t1[e] == 0x20 || t1[e] == 0x0D || t1[e] == 0x09))
          --e;

        // Pre-formatted fragment marker or its escape.
        //
        if (t1[b] == '\\' && (b == e || (b + 1 == e && t1[e] == '\\')))
        {
          // Use Start of Text (0x02) and End of Text (0x03) special
          // characters as pre-formatted fragment markers.
          //
          if (b == e)
          {
            pre = !pre;
            t2 += (pre ? 0x02 : 0x03);
          }
          else
            t2 += "\\\\"; // Keep escaped.
        }
        else if (b <= e)
          t2.append (t1, b, e - b + 1);
      }

      if (more)
      {
        t2 += '\n';
        b = p + 1;
      }
    }

    if (pre)
    {
      cerr << *path_ << ": error: missing pre-formatted fragment end marker "
           << "in documentation string '" << t1 << "'" << endl;
      throw error ();
    }
  }

  // Pass 3: replace every single newline with single space and all multiple
  // newlines (paragraph marker) with a single newline, unless we are in a
  // pre-formatted fragment. Also process escapes in pre-formatted fragmens.
  //
  bool pre (false);
  p = '\0'; // Previous character in pre-formatted fragment.
  for (size_t i (0), n (t2.size ()); i < n; ++i)
  {
    char c (t2[i]);

    if (c == '\n' && !pre)
    {
      size_t j (i);
      for (; i + 1 < n && t2[i + 1] == '\n'; ++i) ;

      if (j != 0 && i + 1 != n) // Strip leading and trailing newlines.
        t3 += i != j ? '\n' : ' ';
    }
    else
    {
      if (c == (pre ? 0x03 : 0x02))
      {
        pre = !pre;

        // Kill "inner" newlines (after opening and before closing '/'
        // markers). Also check for "outer" newlines so that we always
        // have paragraph separation.
        //
        size_t k (t3.size ());
        if (pre)
        {
          if (k != 0 && t3[k - 1] != '\n') // Outer.
          {
            cerr << *path_ << ": error: missing empty line before pre-"
                 << "formatted fragment start marker in documentation "
                 << "string '" << t1 << "'" << endl;
            throw error ();
          }

          ++i; // Skip inner.
        }
        else
        {
          if (t3[k - 1] == '\n') // Could be the same as opening if empty.
            t3.resize (k - 1); // Pop inner.

          if (i + 2 < n && (t2[i + 1] != '\n' || t2[i + 2] != '\n')) // Outer.
          {
            cerr << *path_ << ": error: missing empty line after pre-"
                 << "formatted fragment end marker in documentation "
                 << "string '" << t1 << "'" << endl;
            throw error ();
          }
        }

        t3 += c;
        continue;
      }

      if (pre)
      {
        // In the pre-formatted fragments the only two escapes that we
        // recognize are \" which was handled on pass 1 above and \\ which we
        // handle here.
        //
        if (c == '\\' && p == '\\')
        {
          p = '\0'; // Keep the already added and clear.
          continue;
        }

        p = c;
      }

      t3 += c;
    }
  }

  return t3;
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
