// file      : cli/runtime-source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include <cli/runtime-source.hxx>

using namespace std;

void
generate_runtime_source (context& ctx, bool complete)
{
  ostream& os (ctx.os);

  os << "#include <map>" << endl
     << "#include <set>" << endl
     << "#include <string>" << endl
     << "#include <vector>" << endl
     << "#include <ostream>" << endl
     << "#include <sstream>" << endl;

  if (complete && ctx.options.generate_file_scanner ())
    os << "#include <cstring>" << endl
       << "#include <fstream>" << endl;

  os << endl;

  ctx.ns_open (ctx.cli);

  if (complete)
  {
    string const& os_type (ctx.options.ostream_type ());

    // unknown_option
    //
    os << "// unknown_option" << endl
       << "//" << endl
       << "unknown_option::" << endl
       << "~unknown_option () throw ()"
       << "{"
       << "}"

       << "void unknown_option::" << endl
       << "print (" << os_type << "& os) const"
       << "{"
       << "os << \"unknown option '\" << option ().c_str () << \"'\";"
       << "}"

       << "const char* unknown_option::" << endl
       << "what () const throw ()"
       << "{"
       << "return \"unknown option\";"
       << "}";

    // unknown_argument
    //
    os << "// unknown_argument" << endl
       << "//" << endl
       << "unknown_argument::" << endl
       << "~unknown_argument () throw ()"
       << "{"
       << "}"

       << "void unknown_argument::" << endl
       << "print (" << os_type << "& os) const"
       << "{"
       << "os << \"unknown argument '\" << argument ().c_str () << \"'\";"
       << "}"

       << "const char* unknown_argument::" << endl
       << "what () const throw ()"
       << "{"
       << "return \"unknown argument\";"
       << "}";

    // missing_value
    //
    os << "// missing_value" << endl
       << "//" << endl
       << "missing_value::" << endl
       << "~missing_value () throw ()"
       << "{"
       << "}"

       << "void missing_value::" << endl
       << "print (" << os_type << "& os) const"
       << "{"
       << "os << \"missing value for option '\" << option ().c_str () << \"'\";"
       << "}"

       << "const char* missing_value::" << endl
       << "what () const throw ()"
       << "{"
       << "return \"missing option value\";"
       << "}";

    // invalid_value
    //
    os << "// invalid_value" << endl
       << "//" << endl
       << "invalid_value::" << endl
       << "~invalid_value () throw ()"
       << "{"
       << "}"

       << "void invalid_value::" << endl
       << "print (" << os_type << "& os) const"
       << "{"
       << "os << \"invalid value '\" << value ().c_str () << \"' for " <<
      "option '\"" << endl
       << "   << option ().c_str () << \"'\";"
       << endl
       << "if (!message ().empty ())" << endl
       << "os << \": \" << message ().c_str ();"
       << "}"

       << "const char* invalid_value::" << endl
       << "what () const throw ()"
       << "{"
       << "return \"invalid option value\";"
       << "}";

    // eos_reached
    //
    os << "// eos_reached" << endl
       << "//" << endl
       << "void eos_reached::" << endl
       << "print (" << os_type << "& os) const"
       << "{"
       << "os << what ();"
       << "}"

       << "const char* eos_reached::" << endl
       << "what () const throw ()"
       << "{"
       << "return \"end of argument stream reached\";"
       << "}";

    if (ctx.options.generate_file_scanner ())
    {
      // file_io_failure
      //
      os << "// file_io_failure" << endl
         << "//" << endl
         << "file_io_failure::" << endl
         << "~file_io_failure () throw ()"
         << "{"
         << "}"

         << "void file_io_failure::" << endl
         << "print (" << os_type << "& os) const"
         << "{"
         << "os << \"unable to open file '\" << file ().c_str () << " <<
        "\"' or read failure\";"
         << "}"

         << "const char* file_io_failure::" << endl
         << "what () const throw ()"
         << "{"
         << "return \"unable to open file or read failure\";"
         << "}";

      // unmatched_argument
      //
      os << "// unmatched_quote" << endl
         << "//" << endl
         << "unmatched_quote::" << endl
         << "~unmatched_quote () throw ()"
         << "{"
         << "}"

         << "void unmatched_quote::" << endl
         << "print (" << os_type << "& os) const"
         << "{"
         << "os << \"unmatched quote in argument '\" << " <<
        "argument ().c_str () << \"'\";"
         << "}"

         << "const char* unmatched_quote::" << endl
         << "what () const throw ()"
         << "{"
         << "return \"unmatched quote\";"
         << "}";
    }

    if (ctx.options.generate_group_scanner ())
    {
      // unexpected_group
      //
      os << "// unexpected_group" << endl
         << "//" << endl

         << "unexpected_group::" << endl
         << "~unexpected_group () throw ()"
         << "{"
         << "}"

         << "void unexpected_group::" << endl
         << "print (" << os_type << "& os) const"
         << "{"
         << "os << \"unexpected grouped argument '\" << group_ << \"' \"" << endl
         << "   << \"for argument '\" << argument_ << \"'\";"
         << "}"

         << "const char* unexpected_group::" << endl
         << "what () const throw ()"
         << "{"
         << "return \"unexpected grouped argument\";"
         << "}";

        // group_separator
        //
        os << "// group_separator" << endl
           << "//" << endl

           << "group_separator::" << endl
           << "~group_separator () throw ()"
           << "{"
           << "}"

           << "void group_separator::" << endl
           << "print (" << os_type << "& os) const"
           << "{"
           << "bool ex (!expected_.empty ());"
           << "bool en (!encountered_.empty ());"
           << endl
           << "if (ex)"
           << "{"
           << "os << \"expected group separator '\" << expected_ << \"'\";"
           << "if (en)" << endl
           << "os << \" instead of '\" << encountered_ << \"'\";"
           << "}"
           << "else" << endl
           << "os << \"unexpected group separator '\" << encountered_ << \"'\";"
           << endl
           << "if (en)" << endl
           << "os << \", use '\\\\\" << encountered_ << \"' to escape\";"
           << "}"

           << "const char* group_separator::" << endl
           << "what () const throw ()"
           << "{"
           << "bool ex (!expected_.empty ());"
           << "bool en (!encountered_.empty ());"
           << endl
           << "return en" << endl
           << "  ? ex ? \"wrong group separator\"    : \"unexpected group separator\"" << endl
           << "  : ex ? \"expected group separator\" : \"\";"
           << "}";
    }

    // scanner
    //
    os << "// scanner" << endl
       << "//" << endl
       << "scanner::" << endl
       << "~scanner ()"
       << "{"
       << "}";

    // argv_scanner
    //
    os << "// argv_scanner" << endl
       << "//" << endl

       << "bool argv_scanner::" << endl
       << "more ()"
       << "{"
       << "return i_ < argc_;"
       << "}"

       << "const char* argv_scanner::" << endl
       << "peek ()"
       << "{"
       << "if (i_ < argc_)" << endl
       << "return argv_[i_];"
       << "else" << endl
       << "throw eos_reached ();"
       << "}"

       << "const char* argv_scanner::" << endl
       << "next ()"
       << "{"
       << "if (i_ < argc_)"
       << "{"
       << "const char* r (argv_[i_]);"
       << endl
       << "if (erase_)"
       << "{"
       << "for (int i (i_ + 1); i < argc_; ++i)" << endl
       << "argv_[i - 1] = argv_[i];"
       << endl
       << "--argc_;"
       << "argv_[argc_] = 0;"
       << "}"
       << "else" << endl
       << "++i_;"
       << endl
       << "return r;"
       << "}"
       << "else" << endl
       << "throw eos_reached ();"
       << "}"

       << "void argv_scanner::" << endl
       << "skip ()"
       << "{"
       << "if (i_ < argc_)" << endl
       << "++i_;"
       << "else" << endl
       << "throw eos_reached ();"
       << "}";

    // vector_scanner
    //
    if (ctx.options.generate_vector_scanner ())
    {
      os << "// vector_scanner" << endl
         << "//" << endl

         << "bool vector_scanner::" << endl
         << "more ()"
         << "{"
         << "return i_ < v_.size ();"
         << "}"

         << "const char* vector_scanner::" << endl
         << "peek ()"
         << "{"
         << "if (i_ < v_.size ())" << endl
         << "return v_[i_].c_str ();"
         << "else" << endl
         << "throw eos_reached ();"
         << "}"

         << "const char* vector_scanner::" << endl
         << "next ()"
         << "{"
         << "if (i_ < v_.size ())" << endl
         << "return v_[i_++].c_str ();"
         << "else" << endl
         << "throw eos_reached ();"
         << "}"

         << "void vector_scanner::" << endl
         << "skip ()"
         << "{"
         << "if (i_ < v_.size ())" << endl
         << "++i_;"
         << "else" << endl
         << "throw eos_reached ();"
         << "}";
    }

    // argv_file_scanner
    //
    if (ctx.options.generate_file_scanner ())
    {
      bool sep (!ctx.opt_sep.empty ());

      const string& pfx (ctx.opt_prefix);
      size_t pfx_n (pfx.size ());
      bool comb_values (pfx_n != 0 && !ctx.options.no_combined_values ());

      os << "// argv_file_scanner" << endl
         << "//" << endl

         << "int argv_file_scanner::zero_argc_ = 0;"
         << "std::string argv_file_scanner::empty_string_;"
         << endl

         << "bool argv_file_scanner::" << endl
         << "more ()"
         << "{"
         << "if (!args_.empty ())" << endl
         << "return true;"
         << endl
         << "while (base::more ())"
         << "{"
         << "// See if the next argument is the file option." << endl
         << "//" << endl
         << "const char* a (base::peek ());"
         << "const option_info* oi = 0;"
         << "const char* ov = 0;"
         << endl;

      if (sep)
        os << "if (!skip_)"
           << "{";

      os << "if ((oi = find (a)) != 0)"
         << "{"
         << "base::next ();"
         << endl
         << "if (!base::more ())" << endl
         << "throw missing_value (a);"
         << endl
         << "ov = base::next ();"
         << "}";

      // Handle the combined option/value (--foo=bar). See the option parsing
      // implementation for details.
      //
      if (comb_values)
      {
        os << "else if (std::strncmp (a, \"" << pfx << "\", " <<
          pfx_n << ") == 0)" // It looks like an option.
           << "{"
           <<   "if ((ov = std::strchr (a, '=')) != 0)" // Has '='.
           <<   "{"
           <<     "std::string o (a, 0, ov - a);"
           <<     "if ((oi = find (o.c_str ())) != 0)"
           <<     "{"
           <<       "base::next ();"
           <<       "++ov;" // That's the value.
           <<     "}"
           <<   "}"
           << "}";
      }

      if (sep)
        os << "}";

      os << "if (oi != 0)"
         << "{"
         << "if (oi->search_func != 0)"
         << "{"
         << "std::string f (oi->search_func (ov, oi->arg));"
         << endl
         << "if (!f.empty ())" << endl
         << "load (f);"
         << "}"
         << "else" << endl
         << "load (ov);"
         << endl
         << "if (!args_.empty ())" << endl
         << "return true;"
         << "}"
         << "else"
         << "{";
      if (sep)
        os << "if (!skip_)" << endl
           << "skip_ = (std::strcmp (a, \"" << ctx.opt_sep << "\") == 0);"
           << endl;
      os << "return true;"
         << "}"
         << "}" // while
         << "return false;"
         << "}"

         << "const char* argv_file_scanner::" << endl
         << "peek ()"
         << "{"
         << "if (!more ())" << endl
         << "throw eos_reached ();"
         << endl
         << "return args_.empty () ? base::peek () : args_.front ().value.c_str ();"
         << "}"

         << "const std::string& argv_file_scanner::" << endl
         << "peek_file ()"
         << "{"
         << "if (!more ())" << endl
         << "throw eos_reached ();"
         << endl
         << "return args_.empty () ? empty_string_ : *args_.front ().file;"
         << "}"

         << "std::size_t argv_file_scanner::" << endl
         << "peek_line ()"
         << "{"
         << "if (!more ())" << endl
         << "throw eos_reached ();"
         << endl
         << "return args_.empty () ? 0 : args_.front ().line;"
         << "}"

         << "const char* argv_file_scanner::" << endl
         << "next ()"
         << "{"
         << "if (!more ())" << endl
         << "throw eos_reached ();"
         << endl
         << "if (args_.empty ())" << endl
         << "return base::next ();"
         << "else"
         << "{"
         << "hold_[i_ == 0 ? ++i_ : --i_].swap (args_.front ().value);"
         << "args_.pop_front ();"
         << "return hold_[i_].c_str ();"
         << "}"
         << "}"

         << "void argv_file_scanner::" << endl
         << "skip ()"
         << "{"
         << "if (!more ())" << endl
         << "throw eos_reached ();"
         << endl
         << "if (args_.empty ())" << endl
         << "return base::skip ();"
         << "else" << endl
         << "args_.pop_front ();"
         << "}"

         << "const argv_file_scanner::option_info* argv_file_scanner::" << endl
         << "find (const char* a) const"
         << "{"
         << "for (std::size_t i (0); i < options_count_; ++i)" << endl
         << "if (std::strcmp (a, options_[i].option) == 0)" << endl
         << "return &options_[i];"
         << endl
         << "return 0;"
         << "}"

         << "void argv_file_scanner::" << endl
         << "load (const std::string& file)"
         << "{"
         << "using namespace std;"
         << endl
         << "ifstream is (file.c_str ());"
         << endl
         << "if (!is.is_open ())" << endl
         << "throw file_io_failure (file);"
         << endl
         << "files_.push_back (file);"
         << endl
         << "arg a;"
         << "a.file = &*files_.rbegin ();"
         << endl
         << "for (a.line = 1; !is.eof (); ++a.line)"
         << "{"
         << "string line;"
         << "getline (is, line);"
         << endl
         << "if (is.fail () && !is.eof ())" << endl
         << "throw file_io_failure (file);"
         << endl
         << "string::size_type n (line.size ());"
         << endl
         << "// Trim the line from leading and trailing whitespaces." << endl
         << "//" << endl
         << "if (n != 0)"
         << "{"
         << "const char* f (line.c_str ());"
         << "const char* l (f + n);"
         << endl
         << "const char* of (f);"
         << "while (f < l && (*f == ' ' || *f == '\\t' || *f == '\\r'))" << endl
         << "++f;"
         << endl
         << "--l;"
         << endl
         << "const char* ol (l);"
         << "while (l > f && (*l == ' ' || *l == '\\t' || *l == '\\r'))" << endl
         << "--l;"
         << endl
         << "if (f != of || l != ol)" << endl
         << "line = f <= l ? string (f, l - f + 1) : string ();"
         << "}"
         << "// Ignore empty lines, those that start with #." << endl
         << "//" << endl
         << "if (line.empty () || line[0] == '#')" << endl
         << "continue;"
         << endl
         << "string::size_type p (string::npos);";

      // If we have the option prefix, then only consider lines that start
      // with that as options.
      //
      if (pfx_n != 0)
        os << "if (line.compare (0, " << pfx_n << ", \"" << pfx << "\") == 0)"
           << "{";

      os << "p = line.find (' ');";

      // Handle the combined option/value (--foo=bar). This is a bit tricky
      // since the equal sign can be part of the value (--foo bar=baz).
      //
      // Note that we cannot just pass it along as combined because of
      // quoting (--foo="'bar baz'").
      //
      if (comb_values)
      {
        os << endl
           << "string::size_type q (line.find ('='));"
           << "if (q != string::npos && q < p)" << endl
           <<   "p = q;";
      }

      if (pfx_n != 0)
        os << "}";
      else
        os << endl;

      os << "string s1;"
         << "if (p != string::npos)"
         << "{"
         <<   "s1.assign (line, 0, p);"
         << endl
         << "// Skip leading whitespaces in the argument." << endl
         << "//" << endl;
      if (comb_values)
        os << "if (line[p] == '=')" << endl // Keep whitespaces after '='.
           <<   "++p;"
           << "else"
           << "{";
      os <<     "n = line.size ();"
         <<     "for (++p; p < n; ++p)"
         <<     "{"
         <<       "char c (line[p]);"
         <<       "if (c != ' ' && c != '\\t' && c != '\\r')" << endl
         <<         "break;"
         <<     "}";
      if (comb_values)
        os << "}";
      os << "}";
      if (sep)
        os << "else if (!skip_)" << endl
           <<   "skip_ = (line == \"" << ctx.opt_sep << "\");"
           << endl;

      os << "string s2 (line, p != string::npos ? p : 0);"
         << endl
         << "// If the string (which is an option value or argument) is" << endl
         << "// wrapped in quotes, remove them." << endl
         << "//" << endl
         << "n = s2.size ();"
         << "char cf (s2[0]), cl (s2[n - 1]);"
         << endl
         << "if (cf == '\"' || cf == '\\'' || cl == '\"' || cl == '\\'')"
         << "{"
         <<   "if (n == 1 || cf != cl)" << endl
         <<     "throw unmatched_quote (s2);"
         << endl
         <<   "s2 = string (s2, 1, n - 2);"
         << "}";

      os << "if (!s1.empty ())"
         << "{"
         <<   "// See if this is another file option." << endl
         <<   "//" << endl
         <<   "const option_info* oi;"
         <<   "if (" << (sep ? "!skip_ && " : "") <<
        "(oi = find (s1.c_str ())))" << endl
         <<   "{"
         <<     "if (s2.empty ())" << endl
         <<       "throw missing_value (oi->option);"
         << endl
         <<     "if (oi->search_func != 0)"
         <<     "{"
         <<       "std::string f (oi->search_func (s2.c_str (), oi->arg));"
         <<       "if (!f.empty ())" << endl
         <<         "load (f);"
         <<     "}"
         <<     "else" << endl
         <<       "load (s2);"
         << endl
         <<     "continue;"
         <<   "}"
         <<   "a.value = s1;"
         <<   "args_.push_back (a);"
         << "}"
         << "a.value = s2;"
         << "args_.push_back (a);"
         << "}" // while
         << "}";
    }

    // group_scanner
    //
    if (ctx.options.generate_group_scanner ())
    {
      os << "// group_scanner" << endl
         << "//" << endl

         << "bool group_scanner::" << endl
         << "more ()"
         << "{"
         << "// We don't want to call scan_group() here since that"     << endl
         << "// would invalidate references to previous arguments."     << endl
         << "// But we do need to check that the previous group was"    << endl
         << "// handled."                                               << endl
         << "//"                                                        << endl
         << "if (state_ == scanned)"
         << "{"
         << "if (group_scan_.end () != group_.size ())" << endl
         << "throw unexpected_group (arg_[i_], group_scan_.next ());"
         << "}"
         << "return scan_.more ();"
         << "}"

         << "const char* group_scanner::" << endl
         << "peek ()"
         << "{"
         << "if (state_ != peeked)"  << endl
         << "scan_group (peeked);"
         << "scan_.peek ();"
         << "// Return unescaped." << endl
         << "return arg_[i_].c_str ();"
         << "}"

         << "const char* group_scanner::" << endl
         << "next ()"
         << "{"
         << "if (state_ != peeked)" << endl
         << "scan_group (peeked);"
         << "scan_.next ();"
         << "scan_group (scanned);"
         << "// Return unescaped." << endl
         << "return arg_[i_].c_str ();"
         << "}"

         << "void group_scanner::" << endl
         << "skip ()"
         << "{"
         << "if (state_ != peeked)" << endl
         << "scan_group (peeked);"
         << "scan_.skip ();"
         << "scan_group (skipped);"
         << "}"

         << "void group_scanner::" << endl
         << "scan_group (state st)"
         << "{"
         << "// If the previous argument has been scanned, then make"   << endl
         << "// sure the group has been scanned (handled) as well."     << endl
         << "//"                                                        << endl
         << "if (state_ == scanned)"
         << "{"
         <<   "if (group_scan_.end () != group_.size ())" << endl
         <<     "throw unexpected_group (arg_[i_], group_scan_.next ());"
         << "}"

         << "if (state_ != peeked)"
         << "{"
         <<   "arg_[i_ == 0 ? ++i_ : --i_].clear ();"
         <<   "group_.clear ();"
         <<   "group_scan_.reset ();"
         << "}"

         << "// We recognize all group sequences both before and "      << endl
         << "// after the argument and diagnose any misuse. We may"     << endl
         << "// also have multiple groups:"                             << endl
         << "//"                                                        << endl
         << "// { -x }+ { -y }+ arg"                                    << endl
         << "//"                                                        << endl
         <<                                                                endl
         << "// Using group_ won't cover empty groups."                 << endl
         << "//"                                                        << endl
         << "bool g (false);"
         <<                                                                endl
         << "while (scan_.more ())"
         << "{"
         <<   "const char* a (scan_.peek ());"
         <<   "size_t i (*a == '\\\\' ? 1 : 0);"
         <<   "separator s (sense (a + i));"
         <<                                                                endl
         <<   "if (s == none || i != 0)"
         <<   "{"
         <<     "if (state_ != peeked)" << endl
         <<       "arg_[i_] = a + (s != none ? i : 0);"
         <<     "break;"
         <<   "}"

         <<   "// Start of a leading group for the next argument."      << endl
         <<   "//"                                                      << endl
         <<   "if (s == open && state_ == peeked)" << endl
         <<     "break;"
         <<                                                                endl
         <<   "if (s != (state_ == peeked ? open_plus : open))" << endl
         <<     "throw group_separator (a, \"\");"
         <<                                                                endl
         <<   "g = true;"
         <<                                                                endl
         <<   "// Scan the group until the closing separator."          << endl
         <<   "//"                                                      << endl
         <<   "scan_.next ();"
         <<   "s = none;"
         <<   "while (s == none && scan_.more ())"
         <<   "{"
         <<     "a = scan_.next ();"
         <<     "i = (*a == '\\\\' ? 1 : 0);"
         <<     "s = sense (a + i);"
         <<                                                                endl
         <<     "if (s == none || i != 0)"
         <<     "{"
         <<       "group_.push_back (a + (s != none ? i : 0));"
         <<       "s = none;"
         <<     "}"
         <<   "}"

         <<   "if (s != (state_ == peeked ? close : close_plus))"
         <<   "{"
         <<     "throw group_separator ((s != none        ? a   : \"\")," << endl
         <<                            "(state_ == peeked ? \"}\" : \"}+\"));"
         <<   "}"
         << "}"

         << "// Handle the case where we have seen the leading group"   << endl
         << "// but there are no more arguments."                       << endl
         << "//"                                                        << endl
         << "if (g && state_ != peeked && !scan_.more ())" << endl
         <<   "throw group_separator (\"{\", \"\");"
         <<                                                                endl
         << "state_ = st;"
         << "}";
    }

    // Option description.
    //
    if (ctx.options.generate_description ())
    {
      // options
      //
      os << "void options::" << endl
         << "push_back (const option& o)"
         << "{"
         << "container_type::size_type n (size ());"
         << "container_type::push_back (o);"
         << "map_[o.name ()] = n;"
         << endl
         << "for (option_names::const_iterator i (o.aliases ().begin ());" << endl
         << "i != o.aliases ().end (); ++i)" << endl
         << "map_[*i] = n;"
         << "}";
    }
  }

  // To reduce the number of standard headers we have to include in the
  // generated header file, we always generate the following templates
  // in the source file.
  //
  bool sp (ctx.gen_specifier);
  bool gen_merge (ctx.gen_merge);

  // parser class template & its specializations
  //
  os << "template <typename X>" << endl
     << "struct parser"
     << "{";

  os <<   "static void" << endl
     <<   "parse (X& x, " << (sp ? "bool& xs, " : "") << "scanner& s)"
     <<   "{"
     <<     "using namespace std;"
     <<                                                                    endl
     <<     "const char* o (s.next ());"
     <<     "if (s.more ())"
     <<     "{"
     <<       "string v (s.next ());"
     <<       "istringstream is (v);"
     <<       "if (!(is >> x && is.peek () == istringstream::traits_type::eof ()))" << endl
     <<         "throw invalid_value (o, v);"
     <<     "}"
     <<     "else" << endl
     <<       "throw missing_value (o);";
  if (sp)
    os << endl
       <<   "xs = true;";
  os <<   "}";

  if (gen_merge)
    os << "static void" << endl
       << "merge (X& b, const X& a)"
       << "{"
       <<   "b = a;"
       << "}";

  os << "};";

  // parser<bool>
  //
  os << "template <>" << endl
     << "struct parser<bool>"
     << "{";

  os <<   "static void" << endl
     <<   "parse (bool& x, scanner& s)"
     <<   "{"
     <<     "s.next ();"
     <<     "x = true;"
     <<   "}";

  if (gen_merge)
    os << "static void" << endl
       << "merge (bool& b, const bool&)"
       << "{"
       <<   "b = true;" // We wouldn't be here if a is false.
       << "}";

  os << "};";

  // parser<string>
  //
  os << "template <>" << endl
     << "struct parser<std::string>"
     << "{";

  os <<   "static void" << endl
     <<   "parse (std::string& x, " << (sp ? "bool& xs, " : "") << "scanner& s)"
     <<   "{"
     <<   "const char* o (s.next ());"
     <<                                                                    endl
     <<   "if (s.more ())" << endl
     <<     "x = s.next ();"
     <<   "else" << endl
     <<     "throw missing_value (o);";
  if (sp)
    os <<                                                                  endl
       << "xs = true;";
  os << "}";

  if (gen_merge)
    os << "static void" << endl
       << "merge (std::string& b, const std::string& a)"
       << "{"
       <<   "b = a;"
       << "}";

    os << "};";

  // parser<std::vector<X>>
  //
  os << "template <typename X>" << endl
     << "struct parser<std::vector<X> >"
     << "{";

  os <<   "static void" << endl
     <<   "parse (std::vector<X>& c, " << (sp ? "bool& xs, " : "") << "scanner& s)"
     <<   "{"
     <<     "X x;";
  if (sp)
    os <<   "bool dummy;";
  os <<     "parser<X>::parse (x, " << (sp ? "dummy, " : "") << "s);"
     <<     "c.push_back (x);";
  if (sp)
    os <<   "xs = true;";
  os <<   "}";

  if (gen_merge)
    os << "static void" << endl
       << "merge (std::vector<X>& b, const std::vector<X>& a)"
       << "{"
       <<   "b.insert (b.end (), a.begin (), a.end ());"
       << "}";

  os << "};";

  // parser<std::set<X>>
  //
  os << "template <typename X>" << endl
     << "struct parser<std::set<X> >"
     << "{";

  os <<  "static void" << endl
     <<  "parse (std::set<X>& c, " << (sp ? "bool& xs, " : "") << "scanner& s)"
     <<  "{"
     <<    "X x;";
  if (sp)
    os <<  "bool dummy;";
  os <<    "parser<X>::parse (x, " << (sp ? "dummy, " : "") << "s);"
     <<    "c.insert (x);";
  if (sp)
    os <<  "xs = true;";
  os <<  "}";

  if (gen_merge)
    os << "static void" << endl
       << "merge (std::set<X>& b, const std::set<X>& a)"
       << "{"
       <<   "b.insert (a.begin (), a.end ());"
       << "}";

  os << "};";

  // parser<std::map<K,V>>
  //
  os << "template <typename K, typename V>" << endl
     << "struct parser<std::map<K, V> >"
     << "{";

  os <<   "static void" << endl
     <<   "parse (std::map<K, V>& m, " << (sp ? "bool& xs, " : "") << "scanner& s)"
     <<   "{"
     <<     "const char* o (s.next ());"
     <<                                                                    endl
     <<     "if (s.more ())"
     <<     "{"
     <<       "std::string ov (s.next ());"
     <<       "std::string::size_type p = ov.find ('=');"
     <<                                                                    endl
     <<       "K k = K ();"
     <<       "V v = V ();"
     <<       "std::string kstr (ov, 0, p);"
     <<       "std::string vstr (ov, (p != std::string::npos ? p + 1 : ov.size ()));"
     <<                                                                    endl
     <<       "int ac (2);"
     <<       "char* av[] ="
     <<       "{"
     <<       "const_cast<char*> (o)," << endl
     <<       "0"
     <<       "};";
  if (sp)
    os <<     "bool dummy;";
  os <<       "if (!kstr.empty ())"
     <<       "{"
     <<         "av[1] = const_cast<char*> (kstr.c_str ());"
     <<         "argv_scanner s (0, ac, av);"
     <<         "parser<K>::parse (k, " << (sp ? "dummy, " : "") << "s);"
     <<       "}"
     <<       "if (!vstr.empty ())"
     <<       "{"
     <<         "av[1] = const_cast<char*> (vstr.c_str ());"
     <<         "argv_scanner s (0, ac, av);"
     <<         "parser<V>::parse (v, " << (sp ? "dummy, " : "") << "s);"
     <<       "}"
     <<       "m[k] = v;"
     <<     "}"
     <<     "else" << endl
     <<       "throw missing_value (o);";
  if (sp)
    os << endl
       <<   "xs = true;";
  os <<   "}";

  if (gen_merge)
    os << "static void" << endl
       << "merge (std::map<K, V>& b, const std::map<K, V>& a)"
       << "{"
       <<   "for (typename std::map<K, V>::const_iterator i (a.begin ()); " << endl
       <<         "i != a.end (); "                                         << endl
       <<         "++i)"                                                    << endl
       <<     "b[i->first] = i->second;"
       << "}";

  os << "};";

  // Parser thunk.
  //
  os << "template <typename X, typename T, T X::*M>" << endl
     << "void" << endl
     << "thunk (X& x, scanner& s)"
     << "{"
     << "parser<T>::parse (x.*M, s);"
     << "}";

  if (ctx.gen_specifier)
    os << "template <typename X, typename T, T X::*M, bool X::*S>" << endl
       << "void" << endl
       << "thunk (X& x, scanner& s)"
       << "{"
       << "parser<T>::parse (x.*M, x.*S, s);"
       << "}";

  ctx.ns_close (ctx.cli);
}
