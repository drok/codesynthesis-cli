// file      : cli/parser.test.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include <fstream>
#include <iostream>

#include "parser.hxx"
#include "semantics.hxx"
#include "traversal.hxx"

#undef NDEBUG
#include <cassert>

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
    parser p (include_paths, true /* collect_dependencies */);
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
