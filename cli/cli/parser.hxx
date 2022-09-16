// file      : cli/parser.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_PARSER_HXX
#define CLI_PARSER_HXX

#include <map>
#include <string>
#include <vector>
#include <memory>  // unique_ptr
#include <cstddef> // size_t
#include <istream>

#include <cli/semantics/elements.hxx>
#include <cli/semantics/unit.hxx>

class token;
class lexer;

class parser
{
public:
  typedef std::vector<semantics::path> paths;

  parser (paths const& include_paths, bool collect_dependencies)
      : include_paths_ (include_paths),
        collect_dependencies_ (collect_dependencies) {}

  struct invalid_input {};

  struct parse_result
  {
    std::unique_ptr<semantics::cli_unit> unit;

    // Normalized paths of the main CLI file and files it includes and sources
    // recursively, with the duplicates suppressed.
    //
    paths dependencies;
  };

  parse_result
  parse (std::istream& is, semantics::path const& path);

private:
  struct error {};

  void
  def_unit ();

  void
  source_decl ();

  void
  include_decl ();

  bool
  decl (token&);

  void
  scope_doc (token&);

  void
  namespace_def ();

  void
  class_def ();

  bool
  option_def (token&);

  std::string
  doc_string (const char*, std::size_t);

  bool
  qualified_name (token&, std::string& name);

  bool
  fundamental_type (token&, std::string& name);

private:
  void
  recover (token& t);

private:
  paths const include_paths_;
  bool collect_dependencies_;

  bool valid_;
  semantics::path const* path_;

  lexer* lexer_;

  semantics::cli_unit* root_;
  semantics::cli_unit* cur_;
  semantics::scope* scope_;

  std::size_t doc_count_; // Scope doc counter, see scope_doc() for details.

  // If the entry's value is NULL, then the key refers to a sourced file.
  //
  typedef std::map<semantics::path, semantics::cli_unit*> include_map;
  include_map include_map_;

  paths dependencies_;
};

#endif // CLI_PARSER_HXX
