// file      : cli/runtime-inline.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <cli/runtime-inline.hxx>

using namespace std;

void
generate_runtime_inline (context& ctx)
{
  ostream& os (ctx.os);
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
     << "const std::string& value)" << endl
     << ": option_ (option),"
     << "  value_ (value)"
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

  // argv_scanner
  //
  os << "// argv_scanner" << endl
     << "//" << endl;

  os << inl << "argv_scanner::" << endl
     << "argv_scanner (int& argc, char** argv, bool erase)" << endl
     << ": i_ (1), argc_ (argc), argv_ (argv), erase_ (erase)"
     << "{"
     << "}";

  os << inl << "argv_scanner::" << endl
     << "argv_scanner (int start, int& argc, char** argv, bool erase)" << endl
     << ": i_ (start), argc_ (argc), argv_ (argv), erase_ (erase)"
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
       << "vector_scanner (const std::vector<std::string>& v, " <<
      "std::size_t i)" << endl
       << ": v_ (v), i_ (i)"
       << "{"
       << "}";

    os << inl << "std::size_t vector_scanner::" << endl
       << "end () const"
       << "{"
       << "return i_;"
       << "}";

    os << inl << "void vector_scanner::" << endl
       << "reset (std::size_t i)"
       << "{"
       << "i_ = i;"
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
       << "bool erase)" << endl
       << ": argv_scanner (argc, argv, erase)," << endl
       << "  option_ (option)," << endl
       << "  options_ (&option_info_)," << endl
       << "  options_count_ (1)";
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
       << "bool erase)" << endl
       << ": argv_scanner (start, argc, argv, erase)," << endl
       << "  option_ (option)," << endl
       << "  options_ (&option_info_)," << endl
       << "  options_count_ (1)";
    if (sep)
      os << "," << endl
         << "  skip_ (false)";
    os << "{"
       << "option_info_.option = option_.c_str ();"
       << "option_info_.search_func = 0;"
       << "}";

    os << inl << "argv_file_scanner::" << endl
       << "argv_file_scanner (int& argc," << endl
       << "char** argv," << endl
       << "const option_info* options," << endl
       << "std::size_t options_count," << endl
       << "bool erase)" << endl
       << ": argv_scanner (argc, argv, erase)," << endl
       << "  options_ (options)," << endl
       << "  options_count_ (options_count)";
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
       << "bool erase)" << endl
       << ": argv_scanner (start, argc, argv, erase)," << endl
       << "  options_ (options)," << endl
       << "  options_count_ (options_count)";
    if (sep)
      os << "," << endl
         << "  skip_ (false)";
    os << "{"
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
