// file      : tests/file/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

// Test argv_file_scanner.
//
#include <memory>
#include <string>
#include <iostream>

#include "test.hxx"

using namespace std;

int
main (int argc, char* argv[])
{
  try
  {
    string a (argc > 1 ? argv[1] : "");

    // Special modes.
    //
    // ---- <file>
    // --- <file>
    //
    unique_ptr<cli::scanner> s (
      a == "----" ? new cli::argv_file_scanner (argv[2], "--file") :
      a == "---"  ? new cli::argv_file_scanner (argv[2])           :
      new cli::argv_file_scanner (argc, argv, "--file"));

    while (s->more ())
      cout << s->next () << endl;
  }
  catch (const cli::exception& e)
  {
    cerr << e << endl;
  }
}
