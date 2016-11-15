// file      : cli/option-types.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <istream>

#include <cli/option-types.hxx>

using namespace std;

static const char* cxx_version_[] =
{
  "c++98",
  "c++11"
  "c++14"
};

string cxx_version::
string () const
{
  return cxx_version_[v_];
}

istream&
operator>> (istream& is, cxx_version& v)
{
  string s;
  is >> s;

  if (!is.fail ())
  {
    if (s == "c++98")
      v = cxx_version::cxx98;
    else if (s == "c++11")
      v = cxx_version::cxx11;
    else if (s == "c++14")
      v = cxx_version::cxx14;
    else
      is.setstate (istream::failbit);
  }

  return is;
}
