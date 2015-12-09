// file      : cli/option-types.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_OPTION_TYPES_HXX
#define CLI_OPTION_TYPES_HXX

#include <iosfwd>
#include <string>

struct cxx_version
{
  enum value
  {
    cxx98,
    cxx11,
    cxx14
  };

  cxx_version (value v = value (0)) : v_ (v) {}
  operator value () const {return v_;}

  std::string
  string () const;

private:
  value v_;
};

std::istream&
operator>> (std::istream&, cxx_version&);

#endif // CLI_OPTION_TYPES_HXX
