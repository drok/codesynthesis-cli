// file      : unit-tests/parser/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <fstream>
#include <iostream>

#include <cli/parser.hxx>
#include <cli/semantics.hxx>
#include <cli/traversal.hxx>

using namespace std;

int
main (int argc, char* argv[])
{
  if (argc != 2)
  {
    cerr << "usage: " << argv[0] << " file.cli" << endl;
    return 1;
  }

  try
  {
    semantics::path path (argv[1]);

    ifstream ifs;
    ifs.exceptions (ifstream::failbit | ifstream::badbit);
    ifs.open (path.string ().c_str ());

    parser::paths include_paths;
    parser p (include_paths);
    p.parse (ifs, path);
  }
  catch (semantics::invalid_path const& e)
  {
    cerr << "error: '" << e.path () << "' is not a valid filesystem path"
         << endl;
    return 1;
  }
  catch (parser::invalid_input const&)
  {
    return 1;
  }
}
