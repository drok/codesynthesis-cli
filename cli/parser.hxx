// file      : cli/parser.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
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

  parser (paths const& include_paths): include_paths_ (include_paths) {}

  struct invalid_input {};

  std::unique_ptr<semantics::cli_unit>
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

  bool valid_;
  semantics::path const* path_;

  lexer* lexer_;

  semantics::cli_unit* root_;
  semantics::cli_unit* cur_;
  semantics::scope* scope_;

  std::size_t doc_count_; // Scope doc counter, see scope_doc() for details.

  typedef std::map<semantics::path, semantics::cli_unit*> include_map;
  include_map include_map_;
};

#endif // CLI_PARSER_HXX
