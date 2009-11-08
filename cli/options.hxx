// This code was generated by CLI, a command line interface
// compiler for C++.
//

#ifndef CLI_OPTIONS_HXX
#define CLI_OPTIONS_HXX

#include <iosfwd>
#include <string>
#include <exception>

namespace cli
{
  class unknown_mode
  {
    public:
    enum value
    {
      skip,
      stop,
      fail
    };

    unknown_mode (value v);

    operator value () const 
    {
      return v_;
    }

    private:
    value v_;
  };

  // Exceptions.
  //

  class exception: public std::exception
  {
    public:
    virtual void
    print (std::ostream&) const = 0;
  };

  std::ostream&
  operator<< (std::ostream&, const exception&);

  class unknown_option: public exception
  {
    public:
    virtual
    ~unknown_option () throw ();

    unknown_option (const std::string& option);

    const std::string&
    option () const;

    virtual void
    print (std::ostream&) const;

    virtual const char*
    what () const throw ();

    private:
    std::string option_;
  };

  class unknown_argument: public exception
  {
    public:
    virtual
    ~unknown_argument () throw ();

    unknown_argument (const std::string& argument);

    const std::string&
    argument () const;

    virtual void
    print (std::ostream&) const;

    virtual const char*
    what () const throw ();

    private:
    std::string argument_;
  };

  class missing_value: public exception
  {
    public:
    virtual
    ~missing_value () throw ();

    missing_value (const std::string& option);

    const std::string&
    option () const;

    virtual void
    print (std::ostream&) const;

    virtual const char*
    what () const throw ();

    private:
    std::string option_;
  };

  class invalid_value: public exception
  {
    public:
    virtual
    ~invalid_value () throw ();

    invalid_value (const std::string& option,
                   const std::string& value);

    const std::string&
    option () const;

    const std::string&
    value () const;

    virtual void
    print (std::ostream&) const;

    virtual const char*
    what () const throw ();

    private:
    std::string option_;
    std::string value_;
  };
}

#include <map>

#include <string>

#include <vector>

#include <cstddef>

class options
{
  public:

  options (int argc,
           char** argv,
           ::cli::unknown_mode option = ::cli::unknown_mode::fail,
           ::cli::unknown_mode argument = ::cli::unknown_mode::stop);

  options (int start,
           int argc,
           char** argv,
           ::cli::unknown_mode option = ::cli::unknown_mode::fail,
           ::cli::unknown_mode argument = ::cli::unknown_mode::stop);

  options (int argc,
           char** argv,
           int& end,
           ::cli::unknown_mode option = ::cli::unknown_mode::fail,
           ::cli::unknown_mode argument = ::cli::unknown_mode::stop);

  options (int start,
           int argc,
           char** argv,
           int& end,
           ::cli::unknown_mode option = ::cli::unknown_mode::fail,
           ::cli::unknown_mode argument = ::cli::unknown_mode::stop);

  // Option accessors.
  //

  const bool&
  help () const;

  const bool&
  version () const;

  const std::string&
  output_dir () const;

  const bool&
  suppress_inline () const;

  const bool&
  suppress_usage () const;

  const bool&
  generate_cxx () const;

  const bool&
  generate_man () const;

  const bool&
  generate_html () const;

  const bool&
  stdout () const;

  const std::size_t&
  option_length () const;

  const std::string&
  hxx_suffix () const;

  const std::string&
  ixx_suffix () const;

  const std::string&
  cxx_suffix () const;

  const std::string&
  html_suffix () const;

  const std::string&
  option_prefix () const;

  const std::string&
  option_separator () const;

  const bool&
  include_with_brackets () const;

  const std::string&
  include_prefix () const;

  const std::string&
  guard_prefix () const;

  const std::map<std::string, std::string>&
  reserved_name () const;

  // Print usage information.
  //
  static void
  print_usage (::std::ostream&);

  private:
  int
  _parse (int start,
          int argc,
          char** argv,
          ::cli::unknown_mode option,
          ::cli::unknown_mode argument);

  public:
  bool help_;
  bool version_;
  std::string output_dir_;
  bool suppress_inline_;
  bool suppress_usage_;
  bool generate_cxx_;
  bool generate_man_;
  bool generate_html_;
  bool stdout_;
  std::size_t option_length_;
  std::string hxx_suffix_;
  std::string ixx_suffix_;
  std::string cxx_suffix_;
  std::string html_suffix_;
  std::string option_prefix_;
  std::string option_separator_;
  bool include_with_brackets_;
  std::string include_prefix_;
  std::string guard_prefix_;
  std::map<std::string, std::string> reserved_name_;
};

#include "options.ixx"

#endif // CLI_OPTIONS_HXX
