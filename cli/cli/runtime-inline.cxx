// file      : cli/runtime-inline.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include "runtime-inline.hxx"

using namespace std;

void
generate_runtime_inline (context& ctx)
{
  ostream& os (ctx.os);

  os << "#include <cassert>" << endl
     << endl;

  string const& inl (ctx.inl);
  string const& os_type (ctx.options.ostream_type ());

  ctx.ns_open (ctx.cli);

  // usage_para
  //
  if (!ctx.options.suppress_usage ())
    os << "// usage_para" << endl
       << "//" << endl

       << inl << "usage_para::" << endl
       << "usage_para (value v)" << endl
       << ": v_ (v)"
       << "{"
       << "}";

  // unknown_mode
  //
  os << "// unknown_mode" << endl
     << "//" << endl

     << inl << "unknown_mode::" << endl
     << "unknown_mode (value v)" << endl
     << ": v_ (v)"
     << "{"
     << "}";

  // exception
  //
  os << "// exception" << endl
     << "//" << endl

     << inl << os_type << "&" << endl
     << "operator<< (" << os_type << "& os, const exception& e)"
     << "{"
     << "e.print (os);"
     << "return os;"
     << "}";

  // unknown_option
  //
  os << "// unknown_option" << endl
     << "//" << endl

     << inl << "unknown_option::" << endl
     << "unknown_option (const std::string& option)" << endl
     << ": option_ (option)"
     << "{"
     << "}"

     << inl << "const std::string& unknown_option::" << endl
     << "option () const"
     << "{"
     << "return option_;"
     << "}";

  // unknown_argument
  //
  os << "// unknown_argument" << endl
     << "//" << endl

     << inl << "unknown_argument::" << endl
     << "unknown_argument (const std::string& argument)" << endl
     << ": argument_ (argument)"
     << "{"
     << "}"

     << inl << "const std::string& unknown_argument::" << endl
     << "argument () const"
     << "{"
     << "return argument_;"
     << "}";

  // missing_value
  //
  os << "// missing_value" << endl
     << "//" << endl

     << inl << "missing_value::" << endl
     << "missing_value (const std::string& option)" << endl
     << ": option_ (option)"
     << "{"
     << "}"

     << inl << "const std::string& missing_value::" << endl
     << "option () const"
     << "{"
     << "return option_;"
     << "}";

  // invalid_value
  //
  os << "// invalid_value" << endl
     << "//" << endl

     << inl << "invalid_value::" << endl
     << "invalid_value (const std::string& option," << endl
     << "const std::string& value," << endl
     << "const std::string& message)" << endl
     << ": option_ (option)," << endl
     << "  value_ (value)," << endl
     << "  message_ (message)"
     << "{"
     << "}"

     << inl << "const std::string& invalid_value::" << endl
     << "option () const"
     << "{"
     << "return option_;"
     << "}"

     << inl << "const std::string& invalid_value::" << endl
     << "value () const"
     << "{"
     << "return value_;"
     << "}"

     << inl << "const std::string& invalid_value::" << endl
     << "message () const"
     << "{"
     << "return message_;"
     << "}";

  if (ctx.options.generate_file_scanner ())
  {
    // file_io_failure
    //
    os << "// file_io_failure" << endl
       << "//" << endl

       << inl << "file_io_failure::" << endl
       << "file_io_failure (const std::string& file)" << endl
       << ": file_ (file)"
       << "{"
       << "}"

       << inl << "const std::string& file_io_failure::" << endl
       << "file () const"
       << "{"
       << "return file_;"
       << "}";

    // unmatched_option
    //
    os << "// unmatched_quote" << endl
       << "//" << endl

       << inl << "unmatched_quote::" << endl
       << "unmatched_quote (const std::string& argument)" << endl
       << ": argument_ (argument)"
       << "{"
       << "}"

       << inl << "const std::string& unmatched_quote::" << endl
       << "argument () const"
       << "{"
       << "return argument_;"
       << "}";
  }

  if (ctx.options.generate_group_scanner ())
  {
    // unexpected_group
    //
    os << "// unexpected_group" << endl
       << "//" << endl

       << inl << "unexpected_group::" << endl
       << "unexpected_group (const std::string& argument," << endl
       <<                   "const std::string& group)" << endl
       << ": argument_ (argument), group_ (group)"
       << "{"
       << "}"

       << inl << "const std::string& unexpected_group::" << endl
       << "argument () const"
       << "{"
       << "return argument_;"
       << "}"

       << inl << "const std::string& unexpected_group::" << endl
       << "group () const"
       << "{"
       << "return group_;"
       << "}";

    // group_separator
    //
    os << "// group_separator" << endl
       << "//" << endl

       << inl << "group_separator::" << endl
       << "group_separator (const std::string& encountered," << endl
       <<                  "const std::string& expected)" << endl
       << ": encountered_ (encountered), expected_ (expected)"
       << "{"
       << "}"

       << inl << "const std::string& group_separator::" << endl
       << "encountered () const"
       << "{"
       << "return encountered_;"
       << "}"

       << inl << "const std::string& group_separator::" << endl
       << "expected () const"
       << "{"
       << "return expected_;"
       << "}";
  }

  // argv_scanner
  //
  os << "// argv_scanner" << endl
     << "//" << endl;

  os << inl << "argv_scanner::" << endl
     << "argv_scanner (int& argc," << endl
     << "char** argv," << endl
     << "bool erase," << endl
     << "std::size_t sp)" << endl
     << ": start_position_ (sp + 1)," << endl
     << "  i_ (1)," << endl
     << "  argc_ (argc)," << endl
     << "  argv_ (argv)," << endl
     << "  erase_ (erase)"
     << "{"
     << "}";

  os << inl << "argv_scanner::" << endl
     << "argv_scanner (int start," << endl
     << "int& argc," << endl
     << "char** argv," << endl
     << "bool erase," << endl
     << "std::size_t sp)" << endl
     << ": start_position_ (sp + static_cast<std::size_t> (start))," << endl
     << "  i_ (start)," << endl
     << "  argc_ (argc)," << endl
     << "  argv_ (argv)," << endl
     << "  erase_ (erase)"
     << "{"
     << "}";

  os << inl << "int argv_scanner::" << endl
     << "end () const"
     << "{"
     << "return i_;"
     << "}";

  // vector_scanner
  //
  if (ctx.options.generate_vector_scanner ())
  {
    os << "// vector_scanner" << endl
       << "//" << endl;

    os << inl << "vector_scanner::" << endl
       << "vector_scanner (const std::vector<std::string>& v," << endl
       << "std::size_t i," << endl
       << "std::size_t sp)" << endl
       << ": start_position_ (sp), v_ (v), i_ (i)"
       << "{"
       << "}";

    os << inl << "std::size_t vector_scanner::" << endl
       << "end () const"
       << "{"
       << "return i_;"
       << "}";

    os << inl << "void vector_scanner::" << endl
       << "reset (std::size_t i, std::size_t sp)"
       << "{"
       << "i_ = i;"
       << "start_position_ = sp;"
       << "}";
  }

  // argv_file_scanner
  //
  if (ctx.options.generate_file_scanner ())
  {
    bool sep (!ctx.opt_sep.empty ());

    os << "// argv_file_scanner" << endl
       << "//" << endl;

    os << inl << "argv_file_scanner::" << endl
       << "argv_file_scanner (int& argc," << endl
       << "char** argv," << endl
       << "const std::string& option," << endl
       << "bool erase," << endl
       << "std::size_t sp)" << endl
       << ": argv_scanner (argc, argv, erase, sp)," << endl
       << "  option_ (option)," << endl
       << "  options_ (&option_info_)," << endl
       << "  options_count_ (1)," << endl
       << "  i_ (1)";
    if (sep)
      os << "," << endl
         << "  skip_ (false)";
    os << "{"
       << "option_info_.option = option_.c_str ();"
       << "option_info_.search_func = 0;"
       << "}";

    os << inl << "argv_file_scanner::" << endl
       << "argv_file_scanner (int start," << endl
       << "int& argc," << endl
       << "char** argv," << endl
       << "const std::string& option," << endl
       << "bool erase," << endl
       << "std::size_t sp)" << endl
       << ": argv_scanner (start, argc, argv, erase, sp)," << endl
       << "  option_ (option)," << endl
       << "  options_ (&option_info_)," << endl
       << "  options_count_ (1)," << endl
       << "  i_ (1)";
    if (sep)
      os << "," << endl
         << "  skip_ (false)";
    os << "{"
       << "option_info_.option = option_.c_str ();"
       << "option_info_.search_func = 0;"
       << "}";

    os << inl << "argv_file_scanner::" << endl
       << "argv_file_scanner (const std::string& file," << endl
       << "const std::string& option," << endl
       << "std::size_t sp)" << endl
       << ": argv_scanner (0, zero_argc_, 0, sp)," << endl
       << "  option_ (option)," << endl
       << "  options_ (&option_info_)," << endl
       << "  options_count_ (1)," << endl
       << "  i_ (1)";
    if (sep)
      os << "," << endl
         << "  skip_ (false)";
    os << "{"
       << "option_info_.option = option_.c_str ();"
       << "option_info_.search_func = 0;"
       << endl
       << "load (file);"
       << "}";

    os << inl << "argv_file_scanner::" << endl
       << "argv_file_scanner (int& argc," << endl
       << "char** argv," << endl
       << "const option_info* options," << endl
       << "std::size_t options_count," << endl
       << "bool erase," << endl
       << "std::size_t sp)" << endl
       << ": argv_scanner (argc, argv, erase, sp)," << endl
       << "  options_ (options)," << endl
       << "  options_count_ (options_count)," << endl
       << "  i_ (1)";
    if (sep)
      os << "," << endl
         << "  skip_ (false)";
    os << "{"
       << "}";

    os << inl << "argv_file_scanner::" << endl
       << "argv_file_scanner (int start," << endl
       << "int& argc," << endl
       << "char** argv," << endl
       << "const option_info* options," << endl
       << "std::size_t options_count," << endl
       << "bool erase," << endl
       << "std::size_t sp)" << endl
       << ": argv_scanner (start, argc, argv, erase, sp)," << endl
       << "  options_ (options)," << endl
       << "  options_count_ (options_count)," << endl
       << "  i_ (1)";
    if (sep)
      os << "," << endl
         << "  skip_ (false)";
    os << "{"
       << "}";

    os << inl << "argv_file_scanner::" << endl
       << "argv_file_scanner (const std::string& file," << endl
       << "const option_info* options," << endl
       << "std::size_t options_count," << endl
       << "std::size_t sp)" << endl
       << ": argv_scanner (0, zero_argc_, 0, sp)," << endl
       << "  options_ (options)," << endl
       << "  options_count_ (options_count)," << endl
       << "  i_ (1)";
    if (sep)
      os << "," << endl
         << "  skip_ (false)";
    os << "{"
       << "load (file);"
       << "}";
  }

  // group_scanner
  //
  if (ctx.options.generate_group_scanner ())
  {
    os << "// group_scanner" << endl
       << "//" << endl

       << inl << "group_scanner::" << endl
       << "group_scanner (scanner& s)" << endl
       << ": scan_ (s), state_ (skipped), i_ (1), j_ (0), group_scan_ (group_)"
       << "{"
       << "}"

       << inl << "scanner& group_scanner::" << endl
       << "group ()"
       << "{"
       << "assert (state_ == scanned || state_ == skipped);"
       << "return group_scan_;"
       << "}"

       << inl << "const char* group_scanner::" << endl
       << "escape (const char* a)"
       << "{"
       <<   "switch (sense (a))"
       <<   "{"
       <<   "case separator::none:       break;"
       <<   "case separator::open:       return \"\\\\{\";"
       <<   "case separator::close:      return \"\\\\}\";"
       <<   "case separator::open_plus:  return \"\\\\+{\";"
       <<   "case separator::close_plus: return \"\\\\}+\";"
       <<   "}"
       << "return a;"
       << "}"

       << inl << "group_scanner::separator group_scanner::" << endl
       << "sense (const char* s)"
       << "{"
       <<   "switch (s[0])"
       <<   "{"
       <<   "case '{': return s[1] == '\\0' ? open  : none;"
       <<   "case '}':"
       <<     "{"
       <<       "switch (s[1])"
       <<       "{"
       <<       "case '+': return s[2] == '\\0' ? close_plus  : none;"
       <<       "default:  return s[1] == '\\0' ? close       : none;"
       <<       "}"
       <<     "}"
       <<   "case '+':"
       <<     "{"
       <<       "switch (s[1])"
       <<       "{"
       <<       "case '{': return s[2] == '\\0' ? open_plus  : none;"
       <<       "default:  return                             none;"
       <<       "}"
       <<     "}"
       <<   "}"
       << "return none;"
       << "}";
  }

  // Option description.
  //
  if (ctx.options.generate_description ())
  {
    // option
    //
    os << inl << "const std::string& option::" << endl
       << "name () const"
       << "{"
       << "return name_;"
       << "}";

    os << inl << "const option_names& option::" << endl
       << "aliases () const"
       << "{"
       << "return aliases_;"
       << "}";

    os << inl << "bool option::" << endl
       << "flag () const"
       << "{"
       << "return flag_;"
       << "}";

    os << inl << "const std::string& option::" << endl
       << "default_value () const"
       << "{"
       << "return default_value_;"
       << "}";

    os << inl << "option::" << endl
       << "option ()"
       << "{"
       << "}";

    os << inl << "option::" << endl
       << "option (const std::string& n," << endl
       << "const option_names& a," << endl
       << "bool f," << endl
       << "const std::string& dv)" << endl
       << ": name_ (n), aliases_ (a), flag_ (f), default_value_ (dv)"
       << "{"
       << "}";

    // options
    //
    os << inl << "options::container_type::const_iterator options::" << endl
       << "find (const std::string& name) const"
       << "{"
       << "map_type::const_iterator i (map_.find (name));"
       << "return i != map_.end () ? begin () + i->second : end ();"
       << "}";
  }

  ctx.ns_close (ctx.cli);
}
