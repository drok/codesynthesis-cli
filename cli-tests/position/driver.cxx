// file      : position/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

// Test argument/option position.
//
#include <iostream>

#include "test.hxx"

#undef NDEBUG
#include <cassert>

using namespace std;

int
main (int argc, char* argv[])
{
  try
  {
    cli::argv_file_scanner scan (argc, argv, "--file");
    options ops (scan);

    if (ops.a_specified ())
      cout << ops.a ().second << ": " << "-a " << ops.a ().first << endl;

    for (const pair<int, size_t>& b: ops.b ())
    {
      cout << b.second << ": " << "-b " << b.first << endl;
    }

    while (scan.more ())
    {
      // Note that calling position() inside `cout << ...` depends on order of
      // argument evaluation.
      //
      size_t p (scan.position ());
      cout << p << ": " << scan.next () << endl;
    }

    cout << "max: " << scan.position () << endl;
  }
  catch (const cli::exception& e)
  {
    cerr << e << endl;
  }
}
