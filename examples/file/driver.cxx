// file      : examples/file/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <iostream>
#include <iterator>
#include <algorithm>

#include "options.hxx"

using namespace std;

int
main (int argc, char* argv[])
{
  try
  {
    cli::argv_file_scanner s (argc, argv, "--options-file");
    options o (s);

    cout << "verbosity: " << o.verbose () << endl
         << "values: ";

    copy (o.val ().begin (),
          o.val ().end (),
          ostream_iterator<int> (cout, " "));

    cerr << endl;
  }
  catch (const cli::exception& e)
  {
    cerr << e << endl;
    return 1;
  }
}
