// file      : tests/file/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

// Test argv_file_scanner.
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

    while (s.more ())
      cout << s.next () << endl;
  }
  catch (const cli::exception& e)
  {
    cerr << e << endl;
  }
}
