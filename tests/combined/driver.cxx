// file      : tests/combined/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2018 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

// Test combined flags (-xyz vs -x -y -z) and option values (--foo=bar).
//

#include <iostream>

#include "test.hxx"

using namespace std;

int
main (int argc, char* argv[])
{
  try
  {
    cli::argv_file_scanner s (argc, argv, "--file");
    options o (s);

    if (o.foo_specified ())
      cout << "--foo=" << o.foo () << endl;

    if (o.x () || o.y () || o.z ())
      cout << '-'
           << (o.x () ? "x" : "")
           << (o.y () ? "y" : "")
           << (o.z () ? "z" : "") << endl;

    if (o.xyz ())
      cout << "--xyz" << endl;
  }
  catch (const cli::exception& e)
  {
    cerr << e << endl;
    return 1;
  }
}
