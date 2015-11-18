// file      : cli/context.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_CONTEXT_HXX
#define CLI_CONTEXT_HXX

#include <set>
#include <map>
#include <string>
#include <ostream>
#include <cstddef> // std::size_t

#include <cutl/shared-ptr.hxx>

#include "options.hxx"
#include "semantics.hxx"
#include "traversal.hxx"

using std::endl;

class generation_failed {};

class context
{
public:
  typedef std::size_t size_t;
  typedef std::string string;
  typedef ::options options_type;

public:
  enum usage_type
  {
    ut_none,
    ut_short,
    ut_long,
    ut_both
  };

private:
  struct data;
  cutl::shared_ptr<data> data_;

public:
  std::ostream& os;
  semantics::cli_unit& unit;
  options_type const& options;

  bool modifier;
  bool specifier;
  usage_type usage;

  string const& inl;
  string const& opt_prefix;
  string const& opt_sep;
  string const& cli;

  typedef std::map<string, string> reserved_name_map_type;
  reserved_name_map_type const& reserved_name_map;

  typedef std::set<string> keyword_set_type;
  keyword_set_type const& keyword_set;

private:
  struct data
  {
    string inl_;
    string cli_;
    keyword_set_type keyword_set_;
  };

public:
  // Escape C++ keywords, reserved names, and illegal characters.
  //
  string
  escape (string const&) const;

  // Translate and format the documentation string. Translate converts
  // the <arg>-style constructs to \i{arg}. Format converts the string
  // to the output format.
  //
  enum output_type
  {
    ot_plain,
    ot_html,
    ot_man
  };

  static string
  translate_arg (string const&, std::set<string>&);

  static string
  translate (string const&, std::set<string> const&);

  // If para is true, start a new paragraph.
  //
  string
  format (output_type, string const&, bool para);

  void
  format_line (output_type, string&, const char*, size_t);

public:
  static string const&
  ename (semantics::nameable& n)
  {
    return n.context ().get<string> ("name");
  }

  static string const&
  especifier (semantics::nameable& n)
  {
    return n.context ().get<string> ("specifier");
  }

  static string const&
  emember (semantics::nameable& n)
  {
    return n.context ().get<string> ("member");
  }

  static string const&
  especifier_member (semantics::nameable& n)
  {
    return n.context ().get<string> ("specifier-member");
  }

public:
  // Return fully-qualified C++ or CLI name.
  //
  string
  fq_name (semantics::nameable& n, bool cxx_name = true);

  // Open/close cli namespace.
  //
public:
  void
  cli_open ();

  void
  cli_close ();

public:
  context (std::ostream&, semantics::cli_unit&, options_type const&);

  context (context&);

private:
  context&
  operator= (context const&);
};

// Checks if scope Y names any of X.
//
template <typename X, typename Y>
bool
has (Y& y)
{
  for (semantics::scope::names_iterator i (y.names_begin ()),
         e (y.names_end ()); i != e; ++i)
    if (i->named (). template is_a<X> ())
      return true;

  return false;
}

// Standard namespace traverser.
//
struct namespace_: traversal::namespace_, context
{
  namespace_ (context& c) : context (c) {}

  virtual void
  pre (type&);

  virtual void
  post (type&);
};

#endif // CLI_CONTEXT_HXX
