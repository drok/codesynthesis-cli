// -*- C++ -*-
//
// This file was generated by CLI, a command line interface
// compiler for C++.
//

// Begin prologue.
//
//
// End prologue.

#include <cassert>

namespace cli
{
  // usage_para
  //
  inline usage_para::
  usage_para (value v)
  : v_ (v)
  {
  }

  // unknown_mode
  //
  inline unknown_mode::
  unknown_mode (value v)
  : v_ (v)
  {
  }

  // exception
  //
  inline ::std::ostream&
  operator<< (::std::ostream& os, const exception& e)
  {
    e.print (os);
    return os;
  }

  // unknown_option
  //
  inline unknown_option::
  unknown_option (const std::string& option)
  : option_ (option)
  {
  }

  inline const std::string& unknown_option::
  option () const
  {
    return option_;
  }

  // unknown_argument
  //
  inline unknown_argument::
  unknown_argument (const std::string& argument)
  : argument_ (argument)
  {
  }

  inline const std::string& unknown_argument::
  argument () const
  {
    return argument_;
  }

  // missing_value
  //
  inline missing_value::
  missing_value (const std::string& option)
  : option_ (option)
  {
  }

  inline const std::string& missing_value::
  option () const
  {
    return option_;
  }

  // invalid_value
  //
  inline invalid_value::
  invalid_value (const std::string& option,
                 const std::string& value,
                 const std::string& message)
  : option_ (option),
    value_ (value),
    message_ (message)
  {
  }

  inline const std::string& invalid_value::
  option () const
  {
    return option_;
  }

  inline const std::string& invalid_value::
  value () const
  {
    return value_;
  }

  inline const std::string& invalid_value::
  message () const
  {
    return message_;
  }

  // file_io_failure
  //
  inline file_io_failure::
  file_io_failure (const std::string& file)
  : file_ (file)
  {
  }

  inline const std::string& file_io_failure::
  file () const
  {
    return file_;
  }

  // unmatched_quote
  //
  inline unmatched_quote::
  unmatched_quote (const std::string& argument)
  : argument_ (argument)
  {
  }

  inline const std::string& unmatched_quote::
  argument () const
  {
    return argument_;
  }

  // argv_scanner
  //
  inline argv_scanner::
  argv_scanner (int& argc,
                char** argv,
                bool erase,
                std::size_t sp)
  : start_position_ (sp + 1),
    i_ (1),
    argc_ (argc),
    argv_ (argv),
    erase_ (erase)
  {
  }

  inline argv_scanner::
  argv_scanner (int start,
                int& argc,
                char** argv,
                bool erase,
                std::size_t sp)
  : start_position_ (sp + static_cast<std::size_t> (start)),
    i_ (start),
    argc_ (argc),
    argv_ (argv),
    erase_ (erase)
  {
  }

  inline int argv_scanner::
  end () const
  {
    return i_;
  }

  // argv_file_scanner
  //
  inline argv_file_scanner::
  argv_file_scanner (int& argc,
                     char** argv,
                     const std::string& option,
                     bool erase,
                     std::size_t sp)
  : argv_scanner (argc, argv, erase, sp),
    option_ (option),
    options_ (&option_info_),
    options_count_ (1),
    i_ (1),
    skip_ (false)
  {
    option_info_.option = option_.c_str ();
    option_info_.search_func = 0;
  }

  inline argv_file_scanner::
  argv_file_scanner (int start,
                     int& argc,
                     char** argv,
                     const std::string& option,
                     bool erase,
                     std::size_t sp)
  : argv_scanner (start, argc, argv, erase, sp),
    option_ (option),
    options_ (&option_info_),
    options_count_ (1),
    i_ (1),
    skip_ (false)
  {
    option_info_.option = option_.c_str ();
    option_info_.search_func = 0;
  }

  inline argv_file_scanner::
  argv_file_scanner (const std::string& file,
                     const std::string& option,
                     std::size_t sp)
  : argv_scanner (0, zero_argc_, 0, sp),
    option_ (option),
    options_ (&option_info_),
    options_count_ (1),
    i_ (1),
    skip_ (false)
  {
    option_info_.option = option_.c_str ();
    option_info_.search_func = 0;

    load (file);
  }

  inline argv_file_scanner::
  argv_file_scanner (int& argc,
                     char** argv,
                     const option_info* options,
                     std::size_t options_count,
                     bool erase,
                     std::size_t sp)
  : argv_scanner (argc, argv, erase, sp),
    options_ (options),
    options_count_ (options_count),
    i_ (1),
    skip_ (false)
  {
  }

  inline argv_file_scanner::
  argv_file_scanner (int start,
                     int& argc,
                     char** argv,
                     const option_info* options,
                     std::size_t options_count,
                     bool erase,
                     std::size_t sp)
  : argv_scanner (start, argc, argv, erase, sp),
    options_ (options),
    options_count_ (options_count),
    i_ (1),
    skip_ (false)
  {
  }

  inline argv_file_scanner::
  argv_file_scanner (const std::string& file,
                     const option_info* options,
                     std::size_t options_count,
                     std::size_t sp)
  : argv_scanner (0, zero_argc_, 0, sp),
    options_ (options),
    options_count_ (options_count),
    i_ (1),
    skip_ (false)
  {
    load (file);
  }
}

// options
//

inline const std::uint64_t& options::
build2_metadata () const
{
  return this->build2_metadata_;
}

inline std::uint64_t& options::
build2_metadata ()
{
  return this->build2_metadata_;
}

inline void options::
build2_metadata (const std::uint64_t& x)
{
  this->build2_metadata_ = x;
}

inline bool options::
build2_metadata_specified () const
{
  return this->build2_metadata_specified_;
}

inline void options::
build2_metadata_specified (bool x)
{
  this->build2_metadata_specified_ = x;
}

inline const bool& options::
help () const
{
  return this->help_;
}

inline bool& options::
help ()
{
  return this->help_;
}

inline void options::
help (const bool& x)
{
  this->help_ = x;
}

inline const bool& options::
version () const
{
  return this->version_;
}

inline bool& options::
version ()
{
  return this->version_;
}

inline void options::
version (const bool& x)
{
  this->version_ = x;
}

inline const std::vector<std::string>& options::
include_path () const
{
  return this->include_path_;
}

inline std::vector<std::string>& options::
include_path ()
{
  return this->include_path_;
}

inline void options::
include_path (const std::vector<std::string>& x)
{
  this->include_path_ = x;
}

inline bool options::
include_path_specified () const
{
  return this->include_path_specified_;
}

inline void options::
include_path_specified (bool x)
{
  this->include_path_specified_ = x;
}

inline const std::string& options::
output_dir () const
{
  return this->output_dir_;
}

inline std::string& options::
output_dir ()
{
  return this->output_dir_;
}

inline void options::
output_dir (const std::string& x)
{
  this->output_dir_ = x;
}

inline bool options::
output_dir_specified () const
{
  return this->output_dir_specified_;
}

inline void options::
output_dir_specified (bool x)
{
  this->output_dir_specified_ = x;
}

inline const cxx_version& options::
std () const
{
  return this->std_;
}

inline cxx_version& options::
std ()
{
  return this->std_;
}

inline void options::
std (const cxx_version& x)
{
  this->std_ = x;
}

inline bool options::
std_specified () const
{
  return this->std_specified_;
}

inline void options::
std_specified (bool x)
{
  this->std_specified_ = x;
}

inline const bool& options::
generate_modifier () const
{
  return this->generate_modifier_;
}

inline bool& options::
generate_modifier ()
{
  return this->generate_modifier_;
}

inline void options::
generate_modifier (const bool& x)
{
  this->generate_modifier_ = x;
}

inline const bool& options::
generate_specifier () const
{
  return this->generate_specifier_;
}

inline bool& options::
generate_specifier ()
{
  return this->generate_specifier_;
}

inline void options::
generate_specifier (const bool& x)
{
  this->generate_specifier_ = x;
}

inline const bool& options::
generate_parse () const
{
  return this->generate_parse_;
}

inline bool& options::
generate_parse ()
{
  return this->generate_parse_;
}

inline void options::
generate_parse (const bool& x)
{
  this->generate_parse_ = x;
}

inline const bool& options::
generate_merge () const
{
  return this->generate_merge_;
}

inline bool& options::
generate_merge ()
{
  return this->generate_merge_;
}

inline void options::
generate_merge (const bool& x)
{
  this->generate_merge_ = x;
}

inline const bool& options::
generate_description () const
{
  return this->generate_description_;
}

inline bool& options::
generate_description ()
{
  return this->generate_description_;
}

inline void options::
generate_description (const bool& x)
{
  this->generate_description_ = x;
}

inline const bool& options::
generate_file_scanner () const
{
  return this->generate_file_scanner_;
}

inline bool& options::
generate_file_scanner ()
{
  return this->generate_file_scanner_;
}

inline void options::
generate_file_scanner (const bool& x)
{
  this->generate_file_scanner_ = x;
}

inline const bool& options::
generate_vector_scanner () const
{
  return this->generate_vector_scanner_;
}

inline bool& options::
generate_vector_scanner ()
{
  return this->generate_vector_scanner_;
}

inline void options::
generate_vector_scanner (const bool& x)
{
  this->generate_vector_scanner_ = x;
}

inline const bool& options::
generate_group_scanner () const
{
  return this->generate_group_scanner_;
}

inline bool& options::
generate_group_scanner ()
{
  return this->generate_group_scanner_;
}

inline void options::
generate_group_scanner (const bool& x)
{
  this->generate_group_scanner_ = x;
}

inline const bool& options::
suppress_inline () const
{
  return this->suppress_inline_;
}

inline bool& options::
suppress_inline ()
{
  return this->suppress_inline_;
}

inline void options::
suppress_inline (const bool& x)
{
  this->suppress_inline_ = x;
}

inline const bool& options::
suppress_cli () const
{
  return this->suppress_cli_;
}

inline bool& options::
suppress_cli ()
{
  return this->suppress_cli_;
}

inline void options::
suppress_cli (const bool& x)
{
  this->suppress_cli_ = x;
}

inline const std::string& options::
cli_namespace () const
{
  return this->cli_namespace_;
}

inline std::string& options::
cli_namespace ()
{
  return this->cli_namespace_;
}

inline void options::
cli_namespace (const std::string& x)
{
  this->cli_namespace_ = x;
}

inline bool options::
cli_namespace_specified () const
{
  return this->cli_namespace_specified_;
}

inline void options::
cli_namespace_specified (bool x)
{
  this->cli_namespace_specified_ = x;
}

inline const std::string& options::
ostream_type () const
{
  return this->ostream_type_;
}

inline std::string& options::
ostream_type ()
{
  return this->ostream_type_;
}

inline void options::
ostream_type (const std::string& x)
{
  this->ostream_type_ = x;
}

inline bool options::
ostream_type_specified () const
{
  return this->ostream_type_specified_;
}

inline void options::
ostream_type_specified (bool x)
{
  this->ostream_type_specified_ = x;
}

inline const std::string& options::
export_symbol () const
{
  return this->export_symbol_;
}

inline std::string& options::
export_symbol ()
{
  return this->export_symbol_;
}

inline void options::
export_symbol (const std::string& x)
{
  this->export_symbol_ = x;
}

inline bool options::
export_symbol_specified () const
{
  return this->export_symbol_specified_;
}

inline void options::
export_symbol_specified (bool x)
{
  this->export_symbol_specified_ = x;
}

inline const bool& options::
generate_cxx () const
{
  return this->generate_cxx_;
}

inline bool& options::
generate_cxx ()
{
  return this->generate_cxx_;
}

inline void options::
generate_cxx (const bool& x)
{
  this->generate_cxx_ = x;
}

inline const bool& options::
generate_man () const
{
  return this->generate_man_;
}

inline bool& options::
generate_man ()
{
  return this->generate_man_;
}

inline void options::
generate_man (const bool& x)
{
  this->generate_man_ = x;
}

inline const bool& options::
generate_html () const
{
  return this->generate_html_;
}

inline bool& options::
generate_html ()
{
  return this->generate_html_;
}

inline void options::
generate_html (const bool& x)
{
  this->generate_html_ = x;
}

inline const bool& options::
generate_txt () const
{
  return this->generate_txt_;
}

inline bool& options::
generate_txt ()
{
  return this->generate_txt_;
}

inline void options::
generate_txt (const bool& x)
{
  this->generate_txt_ = x;
}

inline const bool& options::
generate_dep () const
{
  return this->generate_dep_;
}

inline bool& options::
generate_dep ()
{
  return this->generate_dep_;
}

inline void options::
generate_dep (const bool& x)
{
  this->generate_dep_ = x;
}

inline const bool& options::
stdout_ () const
{
  return this->stdout__;
}

inline bool& options::
stdout_ ()
{
  return this->stdout__;
}

inline void options::
stdout_ (const bool& x)
{
  this->stdout__ = x;
}

inline const bool& options::
suppress_undocumented () const
{
  return this->suppress_undocumented_;
}

inline bool& options::
suppress_undocumented ()
{
  return this->suppress_undocumented_;
}

inline void options::
suppress_undocumented (const bool& x)
{
  this->suppress_undocumented_ = x;
}

inline const bool& options::
suppress_usage () const
{
  return this->suppress_usage_;
}

inline bool& options::
suppress_usage ()
{
  return this->suppress_usage_;
}

inline void options::
suppress_usage (const bool& x)
{
  this->suppress_usage_ = x;
}

inline const bool& options::
long_usage () const
{
  return this->long_usage_;
}

inline bool& options::
long_usage ()
{
  return this->long_usage_;
}

inline void options::
long_usage (const bool& x)
{
  this->long_usage_ = x;
}

inline const bool& options::
short_usage () const
{
  return this->short_usage_;
}

inline bool& options::
short_usage ()
{
  return this->short_usage_;
}

inline void options::
short_usage (const bool& x)
{
  this->short_usage_ = x;
}

inline const std::string& options::
page_usage () const
{
  return this->page_usage_;
}

inline std::string& options::
page_usage ()
{
  return this->page_usage_;
}

inline void options::
page_usage (const std::string& x)
{
  this->page_usage_ = x;
}

inline bool options::
page_usage_specified () const
{
  return this->page_usage_specified_;
}

inline void options::
page_usage_specified (bool x)
{
  this->page_usage_specified_ = x;
}

inline const std::size_t& options::
option_length () const
{
  return this->option_length_;
}

inline std::size_t& options::
option_length ()
{
  return this->option_length_;
}

inline void options::
option_length (const std::size_t& x)
{
  this->option_length_ = x;
}

inline bool options::
option_length_specified () const
{
  return this->option_length_specified_;
}

inline void options::
option_length_specified (bool x)
{
  this->option_length_specified_ = x;
}

inline const bool& options::
ascii_tree () const
{
  return this->ascii_tree_;
}

inline bool& options::
ascii_tree ()
{
  return this->ascii_tree_;
}

inline void options::
ascii_tree (const bool& x)
{
  this->ascii_tree_ = x;
}

inline const bool& options::
ansi_color () const
{
  return this->ansi_color_;
}

inline bool& options::
ansi_color ()
{
  return this->ansi_color_;
}

inline void options::
ansi_color (const bool& x)
{
  this->ansi_color_ = x;
}

inline const bool& options::
exclude_base () const
{
  return this->exclude_base_;
}

inline bool& options::
exclude_base ()
{
  return this->exclude_base_;
}

inline void options::
exclude_base (const bool& x)
{
  this->exclude_base_ = x;
}

inline const bool& options::
include_base_last () const
{
  return this->include_base_last_;
}

inline bool& options::
include_base_last ()
{
  return this->include_base_last_;
}

inline void options::
include_base_last (const bool& x)
{
  this->include_base_last_ = x;
}

inline const std::map<std::string, std::string>& options::
class_doc () const
{
  return this->class_doc_;
}

inline std::map<std::string, std::string>& options::
class_doc ()
{
  return this->class_doc_;
}

inline void options::
class_doc (const std::map<std::string, std::string>& x)
{
  this->class_doc_ = x;
}

inline bool options::
class_doc_specified () const
{
  return this->class_doc_specified_;
}

inline void options::
class_doc_specified (bool x)
{
  this->class_doc_specified_ = x;
}

inline const std::vector<std::string>& options::
class_ () const
{
  return this->class__;
}

inline std::vector<std::string>& options::
class_ ()
{
  return this->class__;
}

inline void options::
class_ (const std::vector<std::string>& x)
{
  this->class__ = x;
}

inline bool options::
class__specified () const
{
  return this->class__specified_;
}

inline void options::
class__specified (bool x)
{
  this->class__specified_ = x;
}

inline const std::map<std::string, std::string>& options::
docvar () const
{
  return this->docvar_;
}

inline std::map<std::string, std::string>& options::
docvar ()
{
  return this->docvar_;
}

inline void options::
docvar (const std::map<std::string, std::string>& x)
{
  this->docvar_ = x;
}

inline bool options::
docvar_specified () const
{
  return this->docvar_specified_;
}

inline void options::
docvar_specified (bool x)
{
  this->docvar_specified_ = x;
}

inline const std::vector<std::string>& options::
link_regex () const
{
  return this->link_regex_;
}

inline std::vector<std::string>& options::
link_regex ()
{
  return this->link_regex_;
}

inline void options::
link_regex (const std::vector<std::string>& x)
{
  this->link_regex_ = x;
}

inline bool options::
link_regex_specified () const
{
  return this->link_regex_specified_;
}

inline void options::
link_regex_specified (bool x)
{
  this->link_regex_specified_ = x;
}

inline const bool& options::
link_regex_trace () const
{
  return this->link_regex_trace_;
}

inline bool& options::
link_regex_trace ()
{
  return this->link_regex_trace_;
}

inline void options::
link_regex_trace (const bool& x)
{
  this->link_regex_trace_ = x;
}

inline const std::map<char, std::string>& options::
html_heading_map () const
{
  return this->html_heading_map_;
}

inline std::map<char, std::string>& options::
html_heading_map ()
{
  return this->html_heading_map_;
}

inline void options::
html_heading_map (const std::map<char, std::string>& x)
{
  this->html_heading_map_ = x;
}

inline bool options::
html_heading_map_specified () const
{
  return this->html_heading_map_specified_;
}

inline void options::
html_heading_map_specified (bool x)
{
  this->html_heading_map_specified_ = x;
}

inline const bool& options::
omit_link_check () const
{
  return this->omit_link_check_;
}

inline bool& options::
omit_link_check ()
{
  return this->omit_link_check_;
}

inline void options::
omit_link_check (const bool& x)
{
  this->omit_link_check_ = x;
}

inline const std::vector<std::string>& options::
hxx_prologue () const
{
  return this->hxx_prologue_;
}

inline std::vector<std::string>& options::
hxx_prologue ()
{
  return this->hxx_prologue_;
}

inline void options::
hxx_prologue (const std::vector<std::string>& x)
{
  this->hxx_prologue_ = x;
}

inline bool options::
hxx_prologue_specified () const
{
  return this->hxx_prologue_specified_;
}

inline void options::
hxx_prologue_specified (bool x)
{
  this->hxx_prologue_specified_ = x;
}

inline const std::vector<std::string>& options::
ixx_prologue () const
{
  return this->ixx_prologue_;
}

inline std::vector<std::string>& options::
ixx_prologue ()
{
  return this->ixx_prologue_;
}

inline void options::
ixx_prologue (const std::vector<std::string>& x)
{
  this->ixx_prologue_ = x;
}

inline bool options::
ixx_prologue_specified () const
{
  return this->ixx_prologue_specified_;
}

inline void options::
ixx_prologue_specified (bool x)
{
  this->ixx_prologue_specified_ = x;
}

inline const std::vector<std::string>& options::
cxx_prologue () const
{
  return this->cxx_prologue_;
}

inline std::vector<std::string>& options::
cxx_prologue ()
{
  return this->cxx_prologue_;
}

inline void options::
cxx_prologue (const std::vector<std::string>& x)
{
  this->cxx_prologue_ = x;
}

inline bool options::
cxx_prologue_specified () const
{
  return this->cxx_prologue_specified_;
}

inline void options::
cxx_prologue_specified (bool x)
{
  this->cxx_prologue_specified_ = x;
}

inline const std::vector<std::string>& options::
man_prologue () const
{
  return this->man_prologue_;
}

inline std::vector<std::string>& options::
man_prologue ()
{
  return this->man_prologue_;
}

inline void options::
man_prologue (const std::vector<std::string>& x)
{
  this->man_prologue_ = x;
}

inline bool options::
man_prologue_specified () const
{
  return this->man_prologue_specified_;
}

inline void options::
man_prologue_specified (bool x)
{
  this->man_prologue_specified_ = x;
}

inline const std::vector<std::string>& options::
html_prologue () const
{
  return this->html_prologue_;
}

inline std::vector<std::string>& options::
html_prologue ()
{
  return this->html_prologue_;
}

inline void options::
html_prologue (const std::vector<std::string>& x)
{
  this->html_prologue_ = x;
}

inline bool options::
html_prologue_specified () const
{
  return this->html_prologue_specified_;
}

inline void options::
html_prologue_specified (bool x)
{
  this->html_prologue_specified_ = x;
}

inline const std::vector<std::string>& options::
txt_prologue () const
{
  return this->txt_prologue_;
}

inline std::vector<std::string>& options::
txt_prologue ()
{
  return this->txt_prologue_;
}

inline void options::
txt_prologue (const std::vector<std::string>& x)
{
  this->txt_prologue_ = x;
}

inline bool options::
txt_prologue_specified () const
{
  return this->txt_prologue_specified_;
}

inline void options::
txt_prologue_specified (bool x)
{
  this->txt_prologue_specified_ = x;
}

inline const std::vector<std::string>& options::
hxx_epilogue () const
{
  return this->hxx_epilogue_;
}

inline std::vector<std::string>& options::
hxx_epilogue ()
{
  return this->hxx_epilogue_;
}

inline void options::
hxx_epilogue (const std::vector<std::string>& x)
{
  this->hxx_epilogue_ = x;
}

inline bool options::
hxx_epilogue_specified () const
{
  return this->hxx_epilogue_specified_;
}

inline void options::
hxx_epilogue_specified (bool x)
{
  this->hxx_epilogue_specified_ = x;
}

inline const std::vector<std::string>& options::
ixx_epilogue () const
{
  return this->ixx_epilogue_;
}

inline std::vector<std::string>& options::
ixx_epilogue ()
{
  return this->ixx_epilogue_;
}

inline void options::
ixx_epilogue (const std::vector<std::string>& x)
{
  this->ixx_epilogue_ = x;
}

inline bool options::
ixx_epilogue_specified () const
{
  return this->ixx_epilogue_specified_;
}

inline void options::
ixx_epilogue_specified (bool x)
{
  this->ixx_epilogue_specified_ = x;
}

inline const std::vector<std::string>& options::
cxx_epilogue () const
{
  return this->cxx_epilogue_;
}

inline std::vector<std::string>& options::
cxx_epilogue ()
{
  return this->cxx_epilogue_;
}

inline void options::
cxx_epilogue (const std::vector<std::string>& x)
{
  this->cxx_epilogue_ = x;
}

inline bool options::
cxx_epilogue_specified () const
{
  return this->cxx_epilogue_specified_;
}

inline void options::
cxx_epilogue_specified (bool x)
{
  this->cxx_epilogue_specified_ = x;
}

inline const std::vector<std::string>& options::
man_epilogue () const
{
  return this->man_epilogue_;
}

inline std::vector<std::string>& options::
man_epilogue ()
{
  return this->man_epilogue_;
}

inline void options::
man_epilogue (const std::vector<std::string>& x)
{
  this->man_epilogue_ = x;
}

inline bool options::
man_epilogue_specified () const
{
  return this->man_epilogue_specified_;
}

inline void options::
man_epilogue_specified (bool x)
{
  this->man_epilogue_specified_ = x;
}

inline const std::vector<std::string>& options::
html_epilogue () const
{
  return this->html_epilogue_;
}

inline std::vector<std::string>& options::
html_epilogue ()
{
  return this->html_epilogue_;
}

inline void options::
html_epilogue (const std::vector<std::string>& x)
{
  this->html_epilogue_ = x;
}

inline bool options::
html_epilogue_specified () const
{
  return this->html_epilogue_specified_;
}

inline void options::
html_epilogue_specified (bool x)
{
  this->html_epilogue_specified_ = x;
}

inline const std::vector<std::string>& options::
txt_epilogue () const
{
  return this->txt_epilogue_;
}

inline std::vector<std::string>& options::
txt_epilogue ()
{
  return this->txt_epilogue_;
}

inline void options::
txt_epilogue (const std::vector<std::string>& x)
{
  this->txt_epilogue_ = x;
}

inline bool options::
txt_epilogue_specified () const
{
  return this->txt_epilogue_specified_;
}

inline void options::
txt_epilogue_specified (bool x)
{
  this->txt_epilogue_specified_ = x;
}

inline const std::string& options::
hxx_prologue_file () const
{
  return this->hxx_prologue_file_;
}

inline std::string& options::
hxx_prologue_file ()
{
  return this->hxx_prologue_file_;
}

inline void options::
hxx_prologue_file (const std::string& x)
{
  this->hxx_prologue_file_ = x;
}

inline bool options::
hxx_prologue_file_specified () const
{
  return this->hxx_prologue_file_specified_;
}

inline void options::
hxx_prologue_file_specified (bool x)
{
  this->hxx_prologue_file_specified_ = x;
}

inline const std::string& options::
ixx_prologue_file () const
{
  return this->ixx_prologue_file_;
}

inline std::string& options::
ixx_prologue_file ()
{
  return this->ixx_prologue_file_;
}

inline void options::
ixx_prologue_file (const std::string& x)
{
  this->ixx_prologue_file_ = x;
}

inline bool options::
ixx_prologue_file_specified () const
{
  return this->ixx_prologue_file_specified_;
}

inline void options::
ixx_prologue_file_specified (bool x)
{
  this->ixx_prologue_file_specified_ = x;
}

inline const std::string& options::
cxx_prologue_file () const
{
  return this->cxx_prologue_file_;
}

inline std::string& options::
cxx_prologue_file ()
{
  return this->cxx_prologue_file_;
}

inline void options::
cxx_prologue_file (const std::string& x)
{
  this->cxx_prologue_file_ = x;
}

inline bool options::
cxx_prologue_file_specified () const
{
  return this->cxx_prologue_file_specified_;
}

inline void options::
cxx_prologue_file_specified (bool x)
{
  this->cxx_prologue_file_specified_ = x;
}

inline const std::string& options::
man_prologue_file () const
{
  return this->man_prologue_file_;
}

inline std::string& options::
man_prologue_file ()
{
  return this->man_prologue_file_;
}

inline void options::
man_prologue_file (const std::string& x)
{
  this->man_prologue_file_ = x;
}

inline bool options::
man_prologue_file_specified () const
{
  return this->man_prologue_file_specified_;
}

inline void options::
man_prologue_file_specified (bool x)
{
  this->man_prologue_file_specified_ = x;
}

inline const std::string& options::
html_prologue_file () const
{
  return this->html_prologue_file_;
}

inline std::string& options::
html_prologue_file ()
{
  return this->html_prologue_file_;
}

inline void options::
html_prologue_file (const std::string& x)
{
  this->html_prologue_file_ = x;
}

inline bool options::
html_prologue_file_specified () const
{
  return this->html_prologue_file_specified_;
}

inline void options::
html_prologue_file_specified (bool x)
{
  this->html_prologue_file_specified_ = x;
}

inline const std::string& options::
txt_prologue_file () const
{
  return this->txt_prologue_file_;
}

inline std::string& options::
txt_prologue_file ()
{
  return this->txt_prologue_file_;
}

inline void options::
txt_prologue_file (const std::string& x)
{
  this->txt_prologue_file_ = x;
}

inline bool options::
txt_prologue_file_specified () const
{
  return this->txt_prologue_file_specified_;
}

inline void options::
txt_prologue_file_specified (bool x)
{
  this->txt_prologue_file_specified_ = x;
}

inline const std::string& options::
hxx_epilogue_file () const
{
  return this->hxx_epilogue_file_;
}

inline std::string& options::
hxx_epilogue_file ()
{
  return this->hxx_epilogue_file_;
}

inline void options::
hxx_epilogue_file (const std::string& x)
{
  this->hxx_epilogue_file_ = x;
}

inline bool options::
hxx_epilogue_file_specified () const
{
  return this->hxx_epilogue_file_specified_;
}

inline void options::
hxx_epilogue_file_specified (bool x)
{
  this->hxx_epilogue_file_specified_ = x;
}

inline const std::string& options::
ixx_epilogue_file () const
{
  return this->ixx_epilogue_file_;
}

inline std::string& options::
ixx_epilogue_file ()
{
  return this->ixx_epilogue_file_;
}

inline void options::
ixx_epilogue_file (const std::string& x)
{
  this->ixx_epilogue_file_ = x;
}

inline bool options::
ixx_epilogue_file_specified () const
{
  return this->ixx_epilogue_file_specified_;
}

inline void options::
ixx_epilogue_file_specified (bool x)
{
  this->ixx_epilogue_file_specified_ = x;
}

inline const std::string& options::
cxx_epilogue_file () const
{
  return this->cxx_epilogue_file_;
}

inline std::string& options::
cxx_epilogue_file ()
{
  return this->cxx_epilogue_file_;
}

inline void options::
cxx_epilogue_file (const std::string& x)
{
  this->cxx_epilogue_file_ = x;
}

inline bool options::
cxx_epilogue_file_specified () const
{
  return this->cxx_epilogue_file_specified_;
}

inline void options::
cxx_epilogue_file_specified (bool x)
{
  this->cxx_epilogue_file_specified_ = x;
}

inline const std::string& options::
man_epilogue_file () const
{
  return this->man_epilogue_file_;
}

inline std::string& options::
man_epilogue_file ()
{
  return this->man_epilogue_file_;
}

inline void options::
man_epilogue_file (const std::string& x)
{
  this->man_epilogue_file_ = x;
}

inline bool options::
man_epilogue_file_specified () const
{
  return this->man_epilogue_file_specified_;
}

inline void options::
man_epilogue_file_specified (bool x)
{
  this->man_epilogue_file_specified_ = x;
}

inline const std::string& options::
html_epilogue_file () const
{
  return this->html_epilogue_file_;
}

inline std::string& options::
html_epilogue_file ()
{
  return this->html_epilogue_file_;
}

inline void options::
html_epilogue_file (const std::string& x)
{
  this->html_epilogue_file_ = x;
}

inline bool options::
html_epilogue_file_specified () const
{
  return this->html_epilogue_file_specified_;
}

inline void options::
html_epilogue_file_specified (bool x)
{
  this->html_epilogue_file_specified_ = x;
}

inline const std::string& options::
txt_epilogue_file () const
{
  return this->txt_epilogue_file_;
}

inline std::string& options::
txt_epilogue_file ()
{
  return this->txt_epilogue_file_;
}

inline void options::
txt_epilogue_file (const std::string& x)
{
  this->txt_epilogue_file_ = x;
}

inline bool options::
txt_epilogue_file_specified () const
{
  return this->txt_epilogue_file_specified_;
}

inline void options::
txt_epilogue_file_specified (bool x)
{
  this->txt_epilogue_file_specified_ = x;
}

inline const std::string& options::
output_prefix () const
{
  return this->output_prefix_;
}

inline std::string& options::
output_prefix ()
{
  return this->output_prefix_;
}

inline void options::
output_prefix (const std::string& x)
{
  this->output_prefix_ = x;
}

inline bool options::
output_prefix_specified () const
{
  return this->output_prefix_specified_;
}

inline void options::
output_prefix_specified (bool x)
{
  this->output_prefix_specified_ = x;
}

inline const std::string& options::
output_suffix () const
{
  return this->output_suffix_;
}

inline std::string& options::
output_suffix ()
{
  return this->output_suffix_;
}

inline void options::
output_suffix (const std::string& x)
{
  this->output_suffix_ = x;
}

inline bool options::
output_suffix_specified () const
{
  return this->output_suffix_specified_;
}

inline void options::
output_suffix_specified (bool x)
{
  this->output_suffix_specified_ = x;
}

inline const std::string& options::
hxx_suffix () const
{
  return this->hxx_suffix_;
}

inline std::string& options::
hxx_suffix ()
{
  return this->hxx_suffix_;
}

inline void options::
hxx_suffix (const std::string& x)
{
  this->hxx_suffix_ = x;
}

inline bool options::
hxx_suffix_specified () const
{
  return this->hxx_suffix_specified_;
}

inline void options::
hxx_suffix_specified (bool x)
{
  this->hxx_suffix_specified_ = x;
}

inline const std::string& options::
ixx_suffix () const
{
  return this->ixx_suffix_;
}

inline std::string& options::
ixx_suffix ()
{
  return this->ixx_suffix_;
}

inline void options::
ixx_suffix (const std::string& x)
{
  this->ixx_suffix_ = x;
}

inline bool options::
ixx_suffix_specified () const
{
  return this->ixx_suffix_specified_;
}

inline void options::
ixx_suffix_specified (bool x)
{
  this->ixx_suffix_specified_ = x;
}

inline const std::string& options::
cxx_suffix () const
{
  return this->cxx_suffix_;
}

inline std::string& options::
cxx_suffix ()
{
  return this->cxx_suffix_;
}

inline void options::
cxx_suffix (const std::string& x)
{
  this->cxx_suffix_ = x;
}

inline bool options::
cxx_suffix_specified () const
{
  return this->cxx_suffix_specified_;
}

inline void options::
cxx_suffix_specified (bool x)
{
  this->cxx_suffix_specified_ = x;
}

inline const std::string& options::
man_suffix () const
{
  return this->man_suffix_;
}

inline std::string& options::
man_suffix ()
{
  return this->man_suffix_;
}

inline void options::
man_suffix (const std::string& x)
{
  this->man_suffix_ = x;
}

inline bool options::
man_suffix_specified () const
{
  return this->man_suffix_specified_;
}

inline void options::
man_suffix_specified (bool x)
{
  this->man_suffix_specified_ = x;
}

inline const std::string& options::
html_suffix () const
{
  return this->html_suffix_;
}

inline std::string& options::
html_suffix ()
{
  return this->html_suffix_;
}

inline void options::
html_suffix (const std::string& x)
{
  this->html_suffix_ = x;
}

inline bool options::
html_suffix_specified () const
{
  return this->html_suffix_specified_;
}

inline void options::
html_suffix_specified (bool x)
{
  this->html_suffix_specified_ = x;
}

inline const std::string& options::
txt_suffix () const
{
  return this->txt_suffix_;
}

inline std::string& options::
txt_suffix ()
{
  return this->txt_suffix_;
}

inline void options::
txt_suffix (const std::string& x)
{
  this->txt_suffix_ = x;
}

inline bool options::
txt_suffix_specified () const
{
  return this->txt_suffix_specified_;
}

inline void options::
txt_suffix_specified (bool x)
{
  this->txt_suffix_specified_ = x;
}

inline const std::string& options::
dep_suffix () const
{
  return this->dep_suffix_;
}

inline std::string& options::
dep_suffix ()
{
  return this->dep_suffix_;
}

inline void options::
dep_suffix (const std::string& x)
{
  this->dep_suffix_ = x;
}

inline bool options::
dep_suffix_specified () const
{
  return this->dep_suffix_specified_;
}

inline void options::
dep_suffix_specified (bool x)
{
  this->dep_suffix_specified_ = x;
}

inline const std::string& options::
dep_file () const
{
  return this->dep_file_;
}

inline std::string& options::
dep_file ()
{
  return this->dep_file_;
}

inline void options::
dep_file (const std::string& x)
{
  this->dep_file_ = x;
}

inline bool options::
dep_file_specified () const
{
  return this->dep_file_specified_;
}

inline void options::
dep_file_specified (bool x)
{
  this->dep_file_specified_ = x;
}

inline const std::string& options::
option_prefix () const
{
  return this->option_prefix_;
}

inline std::string& options::
option_prefix ()
{
  return this->option_prefix_;
}

inline void options::
option_prefix (const std::string& x)
{
  this->option_prefix_ = x;
}

inline bool options::
option_prefix_specified () const
{
  return this->option_prefix_specified_;
}

inline void options::
option_prefix_specified (bool x)
{
  this->option_prefix_specified_ = x;
}

inline const std::string& options::
option_separator () const
{
  return this->option_separator_;
}

inline std::string& options::
option_separator ()
{
  return this->option_separator_;
}

inline void options::
option_separator (const std::string& x)
{
  this->option_separator_ = x;
}

inline bool options::
option_separator_specified () const
{
  return this->option_separator_specified_;
}

inline void options::
option_separator_specified (bool x)
{
  this->option_separator_specified_ = x;
}

inline const bool& options::
keep_separator () const
{
  return this->keep_separator_;
}

inline bool& options::
keep_separator ()
{
  return this->keep_separator_;
}

inline void options::
keep_separator (const bool& x)
{
  this->keep_separator_ = x;
}

inline const bool& options::
no_combined_flags () const
{
  return this->no_combined_flags_;
}

inline bool& options::
no_combined_flags ()
{
  return this->no_combined_flags_;
}

inline void options::
no_combined_flags (const bool& x)
{
  this->no_combined_flags_ = x;
}

inline const bool& options::
no_combined_values () const
{
  return this->no_combined_values_;
}

inline bool& options::
no_combined_values ()
{
  return this->no_combined_values_;
}

inline void options::
no_combined_values (const bool& x)
{
  this->no_combined_values_ = x;
}

inline const bool& options::
include_with_brackets () const
{
  return this->include_with_brackets_;
}

inline bool& options::
include_with_brackets ()
{
  return this->include_with_brackets_;
}

inline void options::
include_with_brackets (const bool& x)
{
  this->include_with_brackets_ = x;
}

inline const std::string& options::
include_prefix () const
{
  return this->include_prefix_;
}

inline std::string& options::
include_prefix ()
{
  return this->include_prefix_;
}

inline void options::
include_prefix (const std::string& x)
{
  this->include_prefix_ = x;
}

inline bool options::
include_prefix_specified () const
{
  return this->include_prefix_specified_;
}

inline void options::
include_prefix_specified (bool x)
{
  this->include_prefix_specified_ = x;
}

inline const std::string& options::
guard_prefix () const
{
  return this->guard_prefix_;
}

inline std::string& options::
guard_prefix ()
{
  return this->guard_prefix_;
}

inline void options::
guard_prefix (const std::string& x)
{
  this->guard_prefix_ = x;
}

inline bool options::
guard_prefix_specified () const
{
  return this->guard_prefix_specified_;
}

inline void options::
guard_prefix_specified (bool x)
{
  this->guard_prefix_specified_ = x;
}

inline const std::map<std::string, std::string>& options::
reserved_name () const
{
  return this->reserved_name_;
}

inline std::map<std::string, std::string>& options::
reserved_name ()
{
  return this->reserved_name_;
}

inline void options::
reserved_name (const std::map<std::string, std::string>& x)
{
  this->reserved_name_ = x;
}

inline bool options::
reserved_name_specified () const
{
  return this->reserved_name_specified_;
}

inline void options::
reserved_name_specified (bool x)
{
  this->reserved_name_specified_ = x;
}

inline const std::string& options::
options_file () const
{
  return this->options_file_;
}

inline std::string& options::
options_file ()
{
  return this->options_file_;
}

inline void options::
options_file (const std::string& x)
{
  this->options_file_ = x;
}

inline bool options::
options_file_specified () const
{
  return this->options_file_specified_;
}

inline void options::
options_file_specified (bool x)
{
  this->options_file_specified_ = x;
}

// Begin epilogue.
//
//
// End epilogue.
