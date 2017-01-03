// file      : cli/runtime-header.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <cli/runtime-header.hxx>

using namespace std;

void
generate_runtime_header (context& ctx)
{
  ostream& os (ctx.os);

  if (ctx.options.generate_file_scanner ())
    os << "#include <deque>" << endl;

  if (ctx.options.generate_description ())
    os << "#include <map>" << endl
       << "#include <vector>" << endl;

  os << "#include <iosfwd>" << endl
     << "#include <string>" << endl
     << "#include <cstddef>" << endl
     << "#include <exception>" << endl
     << endl;

  // VC++ and xlC don't like the (void)x expression if x is a reference
  // to an incomplete type. On the other hand, GCC warns that (void*)&x
  // doesn't have any effect.
  //
  os << "#ifndef CLI_POTENTIALLY_UNUSED" << endl
     << "#  if defined(_MSC_VER) || defined(__xlC__)" << endl
     << "#    define CLI_POTENTIALLY_UNUSED(x) (void*)&x" << endl
     << "#  else" << endl
     << "#    define CLI_POTENTIALLY_UNUSED(x) (void)x" << endl
     << "#  endif" << endl
     << "#endif" << endl
     << endl;

  ctx.ns_open (ctx.cli);

  // usage_para
  //
  if (!ctx.options.suppress_usage ())
    os << "class usage_para"
       << "{"
       << "public:" << endl
       << "enum value"
       << "{"
       << "none," << endl
       << "text," << endl
       << "option" << endl
       << "};"
       << "usage_para (value);"
       << endl
       << "operator value () const {return v_;}" // Can't generate outside.
       << "private:" << endl
       << "value v_;"
       << "};";

  // unknown_mode
  //
  os << "class unknown_mode"
     << "{"
     << "public:" << endl
     << "enum value"
     << "{"
     << "skip," << endl
     << "stop," << endl
     << "fail" << endl
     << "};"
     << "unknown_mode (value);"
     << endl
     << "operator value () const {return v_;}" // Can't generate outside.
     << "private:" << endl
     << "value v_;"
     << "};";

  // Exceptions.
  //

  string const& os_type (ctx.options.ostream_type ());

  os << "// Exceptions." << endl
     << "//" << endl
     << endl;

  os << "class exception: public std::exception"
     << "{"
     << "public:" << endl
     << "virtual void" << endl
     << "print (" << os_type << "&) const = 0;"
     << "};";

  os << os_type << "&" << endl
     << "operator<< (" << os_type << "&, const exception&);"
     << endl;

  os << "class unknown_option: public exception"
     << "{"
     << "public:" << endl
     << "virtual" << endl
     << "~unknown_option () throw ();"
     << endl
     << "unknown_option (const std::string& option);"
     << endl
     << "const std::string&" << endl
     << "option () const;"
     << endl
     << "virtual void" << endl
     << "print (" << os_type << "&) const;"
     << endl
     << "virtual const char*" << endl
     << "what () const throw ();"
     << endl
     << "private:" << endl
     << "std::string option_;"
     << "};";

  os << "class unknown_argument: public exception"
     << "{"
     << "public:" << endl
     << "virtual" << endl
     << "~unknown_argument () throw ();"
     << endl
     << "unknown_argument (const std::string& argument);"
     << endl
     << "const std::string&" << endl
     << "argument () const;"
     << endl
     << "virtual void" << endl
     << "print (" << os_type << "&) const;"
     << endl
     << "virtual const char*" << endl
     << "what () const throw ();"
     << endl
     << "private:" << endl
     << "std::string argument_;"
     << "};";

  os << "class missing_value: public exception"
     << "{"
     << "public:" << endl
     << "virtual" << endl
     << "~missing_value () throw ();"
     << endl
     << "missing_value (const std::string& option);"
     << endl
     << "const std::string&" << endl
     << "option () const;"
     << endl
     << "virtual void" << endl
     << "print (" << os_type << "&) const;"
     << endl
     << "virtual const char*" << endl
     << "what () const throw ();"
     << endl
     << "private:" << endl
     << "std::string option_;"
     << "};";

  os << "class invalid_value: public exception"
     << "{"
     << "public:" << endl
     << "virtual" << endl
     << "~invalid_value () throw ();"
     << endl
     << "invalid_value (const std::string& option," << endl
     << "const std::string& value);"
     << endl
     << "const std::string&" << endl
     << "option () const;"
     << endl
     << "const std::string&" << endl
     << "value () const;"
     << endl
     << "virtual void" << endl
     << "print (" << os_type << "&) const;"
     << endl
     << "virtual const char*" << endl
     << "what () const throw ();"
     << endl
     << "private:" << endl
     << "std::string option_;"
     << "std::string value_;"
     << "};";

  os << "class eos_reached: public exception"
     << "{"
     << "public:" << endl
     << "virtual void" << endl
     << "print (" << os_type << "&) const;"
     << endl
     << "virtual const char*" << endl
     << "what () const throw ();"
     << "};";

  if (ctx.options.generate_file_scanner ())
  {
    os << "class file_io_failure: public exception"
       << "{"
       << "public:" << endl
       << "virtual" << endl
       << "~file_io_failure () throw ();"
       << endl
       << "file_io_failure (const std::string& file);"
       << endl
       << "const std::string&" << endl
       << "file () const;"
       << endl
       << "virtual void" << endl
       << "print (" << os_type << "&) const;"
       << endl
       << "virtual const char*" << endl
       << "what () const throw ();"
       << endl
       << "private:" << endl
       << "std::string file_;"
       << "};";

    os << "class unmatched_quote: public exception"
       << "{"
       << "public:" << endl
       << "virtual" << endl
       << "~unmatched_quote () throw ();"
       << endl
       << "unmatched_quote (const std::string& argument);"
       << endl
       << "const std::string&" << endl
       << "argument () const;"
       << endl
       << "virtual void" << endl
       << "print (" << os_type << "&) const;"
       << endl
       << "virtual const char*" << endl
       << "what () const throw ();"
       << endl
       << "private:" << endl
       << "std::string argument_;"
       << "};";
  }

  // scanner
  //
  os << "class scanner"
     << "{"
     << "public:" << endl
     << "virtual" << endl
     << "~scanner ();"
     << endl
     << "virtual bool" << endl
     << "more () = 0;"
     << endl
     << "virtual const char*" << endl
     << "peek () = 0;"
     << endl
     << "virtual const char*" << endl
     << "next () = 0;"
     << endl
     << "virtual void" << endl
     << "skip () = 0;"
     << "};";

  // argv_scanner
  //
  os << "class argv_scanner: public scanner"
     << "{"
     << "public:" << endl
     << "argv_scanner (int& argc, char** argv, bool erase = false);"
     << "argv_scanner (int start, int& argc, char** argv, bool erase = false);"
     << endl
     << "int" << endl
     << "end () const;"
     << endl
     << "virtual bool" << endl
     << "more ();"
     << endl
     << "virtual const char*" << endl
     << "peek ();"
     << endl
     << "virtual const char*" << endl
     << "next ();"
     << endl
     << "virtual void" << endl
     << "skip ();"
     << endl
     << "private:" << endl
     << "int i_;"
     << "int& argc_;"
     << "char** argv_;"
     << "bool erase_;"
     << "};";

  // vector_scanner
  //
  if (ctx.options.generate_vector_scanner ())
  {
    os << "class vector_scanner: public scanner"
       << "{"
       << "public:" << endl
       << "vector_scanner (const std::vector<std::string>&, " <<
      "std::size_t start = 0);"
       << endl
       << "std::size_t" << endl
       << "end () const;"
       << endl
       << "virtual bool" << endl
       << "more ();"
       << endl
       << "virtual const char*" << endl
       << "peek ();"
       << endl
       << "virtual const char*" << endl
       << "next ();"
       << endl
       << "virtual void" << endl
       << "skip ();"
       << endl
       << "private:" << endl
       << "const std::vector<std::string>& v_;"
       << "std::size_t i_;"
       << "};";
  }

  // argv_file_scanner
  //
  if (ctx.options.generate_file_scanner ())
  {
    os << "class argv_file_scanner: public argv_scanner"
       << "{"
       << "public:" << endl
       << "argv_file_scanner (int& argc," << endl
       << "char** argv," << endl
       << "const std::string& option," << endl
       << "bool erase = false);"
       << endl
       << "argv_file_scanner (int start," << endl
       << "int& argc," << endl
       << "char** argv," << endl
       << "const std::string& option," << endl
       << "bool erase = false);"
       << endl
       << "struct option_info"
       << "{"
       << "// If search_func is not NULL, it is called, with the arg" << endl
       << "// value as the second argument, to locate the options file." << endl
       << "// If it returns an empty string, then the file is ignored." << endl
       << "//" << endl
       << "const char* option;"
       << "std::string (*search_func) (const char*, void* arg);"
       << "void* arg;"
       << "};"
       << "argv_file_scanner (int& argc," << endl
       << "char** argv," << endl
       << "const option_info* options," << endl
       << "std::size_t options_count," << endl
       << "bool erase = false);"
       << endl
       << "argv_file_scanner (int start," << endl
       << "int& argc," << endl
       << "char** argv," << endl
       << "const option_info* options," << endl
       << "std::size_t options_count," << endl
       << "bool erase = false);"
       << endl
       << "virtual bool" << endl
       << "more ();"
       << endl
       << "virtual const char*" << endl
       << "peek ();"
       << endl
       << "virtual const char*" << endl
       << "next ();"
       << endl
       << "virtual void" << endl
       << "skip ();"
       << endl
       << "private:" << endl
       << "const option_info*" << endl
       << "find (const char*) const;"
       << endl
       << "void" << endl
       << "load (const std::string& file);"
       << endl
       << "typedef argv_scanner base;"
       << endl
       << "const std::string option_;"
       << "option_info option_info_;"
       << "const option_info* options_;"
       << "std::size_t options_count_;"
       << endl
       << "std::string hold_;"
       << "std::deque<std::string> args_;";

    if (!ctx.opt_sep.empty ())
      os << "bool skip_;";

    os << "};";
  }

  // Option description.
  //
  if (ctx.options.generate_description ())
  {
    os << "typedef std::vector<std::string> option_names;"
       << endl;

    os << "class option"
       << "{"
       << "public:" << endl
       << endl
       << "const std::string&" << endl
       << "name () const;"
       << endl
       << "const option_names&" << endl
       << "aliases () const;"
       << endl
       << "bool" << endl
       << "flag () const;"
       << endl
       << "const std::string&" << endl
       << "default_value () const;"
       << endl
       << "public:"
       << "option ();"
       << "option (const std::string& name," << endl
       << "const option_names& aliases," << endl
       << "bool flag," << endl
       << "const std::string& default_value);"
       << endl
       << "private:" << endl
       << "std::string name_;"
       << "option_names aliases_;"
       << "bool flag_;"
       << "std::string default_value_;"
       << "};";

    os << "class options: public std::vector<option>"
       << "{"
       << "public:" << endl
       << "typedef std::vector<option> container_type;"
       << endl
       << "container_type::const_iterator" << endl
       << "find (const std::string& name) const;"
       << endl
       << "void" << endl
       << "push_back (const option&);"
       << "private:" << endl
       << "typedef std::map<std::string, container_type::size_type> map_type;"
       << "map_type map_;"
       << "};";
  }

  // Parser class template. Provide a forward declaration to allow
  // custom specializations.
  //
  os << "template <typename X>" << endl
     << "struct parser;"
     << endl;

  ctx.ns_close (ctx.cli);
}
