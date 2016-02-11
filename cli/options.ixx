// -*- C++ -*-
//
// This file was generated by CLI, a command line interface
// compiler for C++.
//

// Begin prologue.
//
//
// End prologue.

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
                 const std::string& value)
  : option_ (option),  value_ (value)
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
  argv_scanner (int& argc, char** argv, bool erase)
  : i_ (1), argc_ (argc), argv_ (argv), erase_ (erase)
  {
  }

  inline argv_scanner::
  argv_scanner (int start, int& argc, char** argv, bool erase)
  : i_ (start), argc_ (argc), argv_ (argv), erase_ (erase)
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
                     bool erase)
  : argv_scanner (argc, argv, erase),
    option_ (option),
    options_ (&option_info_),
    options_count_ (1),
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
                     bool erase)
  : argv_scanner (start, argc, argv, erase),
    option_ (option),
    options_ (&option_info_),
    options_count_ (1),
    skip_ (false)
  {
    option_info_.option = option_.c_str ();
    option_info_.search_func = 0;
  }

  inline argv_file_scanner::
  argv_file_scanner (int& argc,
                     char** argv,
                     const option_info* options,
                     std::size_t options_count,
                     bool erase)
  : argv_scanner (argc, argv, erase),
    options_ (options),
    options_count_ (options_count),
    skip_ (false)
  {
  }

  inline argv_file_scanner::
  argv_file_scanner (int start,
                     int& argc,
                     char** argv,
                     const option_info* options,
                     std::size_t options_count,
                     bool erase)
  : argv_scanner (start, argc, argv, erase),
    options_ (options),
    options_count_ (options_count),
    skip_ (false)
  {
  }
}

// options
//

inline const bool& options::
help () const
{
  return this->help_;
}

inline const bool& options::
version () const
{
  return this->version_;
}

inline const std::vector<std::string>& options::
include_path () const
{
  return this->include_path_;
}

inline bool options::
include_path_specified () const
{
  return this->include_path_specified_;
}

inline const std::string& options::
output_dir () const
{
  return this->output_dir_;
}

inline bool options::
output_dir_specified () const
{
  return this->output_dir_specified_;
}

inline const cxx_version& options::
std () const
{
  return this->std_;
}

inline bool options::
std_specified () const
{
  return this->std_specified_;
}

inline const bool& options::
generate_modifier () const
{
  return this->generate_modifier_;
}

inline const bool& options::
generate_specifier () const
{
  return this->generate_specifier_;
}

inline const bool& options::
generate_parse () const
{
  return this->generate_parse_;
}

inline const bool& options::
generate_description () const
{
  return this->generate_description_;
}

inline const bool& options::
generate_file_scanner () const
{
  return this->generate_file_scanner_;
}

inline const bool& options::
suppress_inline () const
{
  return this->suppress_inline_;
}

inline const std::string& options::
cli_namespace () const
{
  return this->cli_namespace_;
}

inline bool options::
cli_namespace_specified () const
{
  return this->cli_namespace_specified_;
}

inline const std::string& options::
ostream_type () const
{
  return this->ostream_type_;
}

inline bool options::
ostream_type_specified () const
{
  return this->ostream_type_specified_;
}

inline const bool& options::
generate_cxx () const
{
  return this->generate_cxx_;
}

inline const bool& options::
generate_man () const
{
  return this->generate_man_;
}

inline const bool& options::
generate_html () const
{
  return this->generate_html_;
}

inline const bool& options::
generate_txt () const
{
  return this->generate_txt_;
}

inline const bool& options::
stdout_ () const
{
  return this->stdout__;
}

inline const bool& options::
suppress_undocumented () const
{
  return this->suppress_undocumented_;
}

inline const bool& options::
suppress_usage () const
{
  return this->suppress_usage_;
}

inline const bool& options::
long_usage () const
{
  return this->long_usage_;
}

inline const bool& options::
short_usage () const
{
  return this->short_usage_;
}

inline const std::string& options::
page_usage () const
{
  return this->page_usage_;
}

inline bool options::
page_usage_specified () const
{
  return this->page_usage_specified_;
}

inline const std::size_t& options::
option_length () const
{
  return this->option_length_;
}

inline bool options::
option_length_specified () const
{
  return this->option_length_specified_;
}

inline const bool& options::
ansi_color () const
{
  return this->ansi_color_;
}

inline const bool& options::
exclude_base () const
{
  return this->exclude_base_;
}

inline const bool& options::
include_base_last () const
{
  return this->include_base_last_;
}

inline const std::map<std::string, std::string>& options::
class_doc () const
{
  return this->class_doc_;
}

inline bool options::
class_doc_specified () const
{
  return this->class_doc_specified_;
}

inline const std::vector<std::string>& options::
class_ () const
{
  return this->class__;
}

inline bool options::
class__specified () const
{
  return this->class__specified_;
}

inline const std::map<std::string, std::string>& options::
docvar () const
{
  return this->docvar_;
}

inline bool options::
docvar_specified () const
{
  return this->docvar_specified_;
}

inline const std::vector<std::string>& options::
link_regex () const
{
  return this->link_regex_;
}

inline bool options::
link_regex_specified () const
{
  return this->link_regex_specified_;
}

inline const bool& options::
link_regex_trace () const
{
  return this->link_regex_trace_;
}

inline const std::map<char, std::string>& options::
html_heading_map () const
{
  return this->html_heading_map_;
}

inline bool options::
html_heading_map_specified () const
{
  return this->html_heading_map_specified_;
}

inline const std::vector<std::string>& options::
hxx_prologue () const
{
  return this->hxx_prologue_;
}

inline bool options::
hxx_prologue_specified () const
{
  return this->hxx_prologue_specified_;
}

inline const std::vector<std::string>& options::
ixx_prologue () const
{
  return this->ixx_prologue_;
}

inline bool options::
ixx_prologue_specified () const
{
  return this->ixx_prologue_specified_;
}

inline const std::vector<std::string>& options::
cxx_prologue () const
{
  return this->cxx_prologue_;
}

inline bool options::
cxx_prologue_specified () const
{
  return this->cxx_prologue_specified_;
}

inline const std::vector<std::string>& options::
man_prologue () const
{
  return this->man_prologue_;
}

inline bool options::
man_prologue_specified () const
{
  return this->man_prologue_specified_;
}

inline const std::vector<std::string>& options::
html_prologue () const
{
  return this->html_prologue_;
}

inline bool options::
html_prologue_specified () const
{
  return this->html_prologue_specified_;
}

inline const std::vector<std::string>& options::
txt_prologue () const
{
  return this->txt_prologue_;
}

inline bool options::
txt_prologue_specified () const
{
  return this->txt_prologue_specified_;
}

inline const std::vector<std::string>& options::
hxx_epilogue () const
{
  return this->hxx_epilogue_;
}

inline bool options::
hxx_epilogue_specified () const
{
  return this->hxx_epilogue_specified_;
}

inline const std::vector<std::string>& options::
ixx_epilogue () const
{
  return this->ixx_epilogue_;
}

inline bool options::
ixx_epilogue_specified () const
{
  return this->ixx_epilogue_specified_;
}

inline const std::vector<std::string>& options::
cxx_epilogue () const
{
  return this->cxx_epilogue_;
}

inline bool options::
cxx_epilogue_specified () const
{
  return this->cxx_epilogue_specified_;
}

inline const std::vector<std::string>& options::
man_epilogue () const
{
  return this->man_epilogue_;
}

inline bool options::
man_epilogue_specified () const
{
  return this->man_epilogue_specified_;
}

inline const std::vector<std::string>& options::
html_epilogue () const
{
  return this->html_epilogue_;
}

inline bool options::
html_epilogue_specified () const
{
  return this->html_epilogue_specified_;
}

inline const std::vector<std::string>& options::
txt_epilogue () const
{
  return this->txt_epilogue_;
}

inline bool options::
txt_epilogue_specified () const
{
  return this->txt_epilogue_specified_;
}

inline const std::string& options::
hxx_prologue_file () const
{
  return this->hxx_prologue_file_;
}

inline bool options::
hxx_prologue_file_specified () const
{
  return this->hxx_prologue_file_specified_;
}

inline const std::string& options::
ixx_prologue_file () const
{
  return this->ixx_prologue_file_;
}

inline bool options::
ixx_prologue_file_specified () const
{
  return this->ixx_prologue_file_specified_;
}

inline const std::string& options::
cxx_prologue_file () const
{
  return this->cxx_prologue_file_;
}

inline bool options::
cxx_prologue_file_specified () const
{
  return this->cxx_prologue_file_specified_;
}

inline const std::string& options::
man_prologue_file () const
{
  return this->man_prologue_file_;
}

inline bool options::
man_prologue_file_specified () const
{
  return this->man_prologue_file_specified_;
}

inline const std::string& options::
html_prologue_file () const
{
  return this->html_prologue_file_;
}

inline bool options::
html_prologue_file_specified () const
{
  return this->html_prologue_file_specified_;
}

inline const std::string& options::
txt_prologue_file () const
{
  return this->txt_prologue_file_;
}

inline bool options::
txt_prologue_file_specified () const
{
  return this->txt_prologue_file_specified_;
}

inline const std::string& options::
hxx_epilogue_file () const
{
  return this->hxx_epilogue_file_;
}

inline bool options::
hxx_epilogue_file_specified () const
{
  return this->hxx_epilogue_file_specified_;
}

inline const std::string& options::
ixx_epilogue_file () const
{
  return this->ixx_epilogue_file_;
}

inline bool options::
ixx_epilogue_file_specified () const
{
  return this->ixx_epilogue_file_specified_;
}

inline const std::string& options::
cxx_epilogue_file () const
{
  return this->cxx_epilogue_file_;
}

inline bool options::
cxx_epilogue_file_specified () const
{
  return this->cxx_epilogue_file_specified_;
}

inline const std::string& options::
man_epilogue_file () const
{
  return this->man_epilogue_file_;
}

inline bool options::
man_epilogue_file_specified () const
{
  return this->man_epilogue_file_specified_;
}

inline const std::string& options::
html_epilogue_file () const
{
  return this->html_epilogue_file_;
}

inline bool options::
html_epilogue_file_specified () const
{
  return this->html_epilogue_file_specified_;
}

inline const std::string& options::
txt_epilogue_file () const
{
  return this->txt_epilogue_file_;
}

inline bool options::
txt_epilogue_file_specified () const
{
  return this->txt_epilogue_file_specified_;
}

inline const std::string& options::
output_prefix () const
{
  return this->output_prefix_;
}

inline bool options::
output_prefix_specified () const
{
  return this->output_prefix_specified_;
}

inline const std::string& options::
output_suffix () const
{
  return this->output_suffix_;
}

inline bool options::
output_suffix_specified () const
{
  return this->output_suffix_specified_;
}

inline const std::string& options::
hxx_suffix () const
{
  return this->hxx_suffix_;
}

inline bool options::
hxx_suffix_specified () const
{
  return this->hxx_suffix_specified_;
}

inline const std::string& options::
ixx_suffix () const
{
  return this->ixx_suffix_;
}

inline bool options::
ixx_suffix_specified () const
{
  return this->ixx_suffix_specified_;
}

inline const std::string& options::
cxx_suffix () const
{
  return this->cxx_suffix_;
}

inline bool options::
cxx_suffix_specified () const
{
  return this->cxx_suffix_specified_;
}

inline const std::string& options::
man_suffix () const
{
  return this->man_suffix_;
}

inline bool options::
man_suffix_specified () const
{
  return this->man_suffix_specified_;
}

inline const std::string& options::
html_suffix () const
{
  return this->html_suffix_;
}

inline bool options::
html_suffix_specified () const
{
  return this->html_suffix_specified_;
}

inline const std::string& options::
txt_suffix () const
{
  return this->txt_suffix_;
}

inline bool options::
txt_suffix_specified () const
{
  return this->txt_suffix_specified_;
}

inline const std::string& options::
option_prefix () const
{
  return this->option_prefix_;
}

inline bool options::
option_prefix_specified () const
{
  return this->option_prefix_specified_;
}

inline const std::string& options::
option_separator () const
{
  return this->option_separator_;
}

inline bool options::
option_separator_specified () const
{
  return this->option_separator_specified_;
}

inline const bool& options::
include_with_brackets () const
{
  return this->include_with_brackets_;
}

inline const std::string& options::
include_prefix () const
{
  return this->include_prefix_;
}

inline bool options::
include_prefix_specified () const
{
  return this->include_prefix_specified_;
}

inline const std::string& options::
guard_prefix () const
{
  return this->guard_prefix_;
}

inline bool options::
guard_prefix_specified () const
{
  return this->guard_prefix_specified_;
}

inline const std::map<std::string, std::string>& options::
reserved_name () const
{
  return this->reserved_name_;
}

inline bool options::
reserved_name_specified () const
{
  return this->reserved_name_specified_;
}

inline const std::string& options::
options_file () const
{
  return this->options_file_;
}

inline bool options::
options_file_specified () const
{
  return this->options_file_specified_;
}

// Begin epilogue.
//
//
// End epilogue.
