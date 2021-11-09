// file      : cli/context.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_CONTEXT_HXX
#define CLI_CONTEXT_HXX

#include <set>
#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <cstddef> // std::size_t

#include <libcutl/re.hxx>
#include <libcutl/shared-ptr.hxx>
#include <libcutl/fs/path.hxx>

#include <cli/options.hxx>
#include <cli/semantics.hxx>
#include <cli/traversal.hxx>

using std::endl;

class generation_failed {};

enum usage
{
  ut_none,
  ut_short,
  ut_long,
  ut_both
};

enum class_doc
{
  cd_default,
  cd_exclude,
  cd_exclude_base,
  cd_short,
  cd_long
};

class context
{
public:
  typedef std::size_t size_t;
  typedef std::string string;

  typedef cutl::fs::path path;
  typedef cutl::fs::invalid_path invalid_path;

  typedef ::options options_type;
  typedef ::usage usage_type;
  typedef ::class_doc class_doc_type;

  // Regex.
  //
  typedef cutl::re::regex regex;
  typedef cutl::re::regexsub regexsub;
  typedef cutl::re::format regex_format;

  typedef std::vector<regexsub> regex_mapping;

private:
  struct data;
  cutl::shared_ptr<data> data_;

public:
  std::ostream& os;
  semantics::cli_unit& unit;
  options_type const& options;

  // Documentation output type.
  //
  enum output_type
  {
    ot_plain,
    ot_html,
    ot_man
  };

  output_type ot;

  bool       gen_modifier;
  bool       gen_specifier;
  bool       gen_parse;
  bool       gen_merge;
  usage_type gen_usage;

  string const& inl;
  string const& opt_prefix;
  string const& opt_sep;
  string const& cli;

  typedef std::map<string, string> reserved_name_map_type;
  reserved_name_map_type const& reserved_name_map;

  typedef std::set<string> keyword_set_type;
  keyword_set_type const& keyword_set;

  regex_mapping const& link_regex;

  typedef std::set<string> id_set_type;
  id_set_type& id_set;
  id_set_type& ref_set;

  // Map of heading ids to heading number (assigned during TOC generation)
  //
  typedef std::map<string, string> heading_map_type;
  heading_map_type& heading_map;

  // TOC phase.
  //
  // 0 - non-TOC
  // 1 - generating TOC after seeing expansion but before restart
  // 2 - generating TOC after restart
  // 0 - non-TOC after seeing expansion after restart
  //
  unsigned short& toc;

  struct toc_entry
  {
    toc_entry (char t = '\0', size_t c = 0): type (t), count (c) {}

    char type;    // Entry type (output type-specific, usually X from \hX).
    size_t count; // Number of sub-entries so far.
  };

  typedef std::vector<toc_entry> toc_stack;
  toc_stack& tocs;

private:
  struct data
  {
    string inl_;
    string cli_;
    keyword_set_type keyword_set_;
    regex_mapping link_regex_;
    id_set_type id_set_;
    id_set_type ref_set_;
    heading_map_type heading_map_;
    unsigned short toc_;
    toc_stack tocs_;
  };

public:
  // Escape C++ keywords, reserved names, and illegal characters.
  //
  string
  escape (string const&) const;

  string
  process_link_target (const string&);

  // Preprocess, translate, and format the documentation string. Translate
  // converts the <arg>-style constructs to \i{arg}. Format converts the
  // string to the output format.
  //
  static void
  preprocess_ascii_tree (string&);

  static string
  translate_arg (string const&, std::set<string>&);

  static string
  translate (string const&, std::set<string> const&);

  // If para is true, start a new paragraph.
  //
  string
  format (semantics::scope&, string const&, bool para);

  void
  format_line (output_type, string&, const char*, size_t);

  // Called when we switch to the TOC mode and when we exit it,
  // respectively.
  //
  string
  start_toc ();

  string
  end_toc ();

  // Make sure each local fragment reference has the corresponding id. Issue
  // diagnostics and throw generation_failed if fails. Otherwise clear the
  // id and ref sets.
  //
  void
  verify_id_ref ();

  // Substitute doc variable expansions ($var$). Var must be a C identifier.
  // If the path is not NULL, then also recognize names that start with either
  // ./ or ../ and treat them as files relative to path. Such file expansions
  // are substituted with the file contents.
  //
  string
  substitute (const string&, const path* = 0);

  // Substitute doc variable expansions (\$var$). Note that it leaves escapes
  // (\\$) as is. Return true if any substitutions have been made, in which
  // case result will contain the expansion result.
  //
  bool
  substitute (semantics::scope&, const char* s, size_t n, string& result);

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

  // Open/close namespace. If last is false, then the last name
  // component is not treated as a namespace. The open function
  // also returns the last name component.
  //
public:
  string
  ns_open (const string& name, bool last = true);

  void
  ns_close (const string& name, bool last = true);

public:
  class_doc_type
  class_doc (semantics::class_&);

public:
  string
  first_sentence (string const&);

public:
  context (std::ostream&,
           output_type,
           semantics::cli_unit&,
           options_type const&);

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
