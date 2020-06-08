// file      : cli/cli.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include <vector>
#include <string>
#include <memory>   // unique_ptr
#include <fstream>
#include <iostream>

#include <cutl/compiler/code-stream.hxx>

#include <cli/semantics/doc.hxx>

#include <cli/options.hxx>
#include <cli/parser.hxx>
#include <cli/generator.hxx>

#include <cli/version.hxx>

using namespace std;
using namespace cutl;

int
main (int argc, char* argv[])
{
  ostream& e (cerr);
  const char* file (0);

  try
  {
    cli::argv_file_scanner scan (argc, argv, "--options-file");
    options ops (scan);

    // Handle --build2-metadata (see also buildfile).
    //
    if (ops.build2_metadata_specified ())
    {
      ostream& o (cout);

      // Note that the export.metadata variable should be the first non-
      // blank/comment line.
      //
      o << "# build2 buildfile cli" << endl
        << "export.metadata = 1 cli" << endl
        << "cli.version = [string] '" << CLI_VERSION_FULL << '\'' << endl
        << "cli.checksum = [string] '" << CLI_VERSION_FULL << '\'' << endl;

      return 0;
    }

    // Handle --version
    //
    if (ops.version ())
    {
      ostream& o (cout);

      o << "CLI (command line interface compiler) " << CLI_VERSION_ID << endl
        << "Copyright (c) " << CLI_COPYRIGHT << "." << endl;

      o << "This is free software; see the source for copying conditions. "
        << "There is NO\nwarranty; not even for MERCHANTABILITY or FITNESS "
        << "FOR A PARTICULAR PURPOSE." << endl;

      return 0;
    }

    // Handle --help
    //
    if (ops.help ())
    {
      ostream& o (cout);

      o << "Usage: " << argv[0] << " [options] file" << endl
        << "Options:" << endl;

      options::print_usage (o);

      return 0;
    }

    if (!scan.more ())
    {
      e << "error: no input file specified" << endl
        << "info: try '" << argv[0] << " --help' for more information" << endl;

      return 1;
    }

    // Extract include search paths.
    //
    parser::paths include_paths;
    for (vector<string>::const_iterator i (ops.include_path ().begin ());
         i != ops.include_path ().end (); ++i)
    {
      // Invalid path exception is handled below.
      //
      include_paths.push_back (semantics::path (*i));
    }

    // Open the input file.
    //
    file = scan.next ();
    semantics::path path (file);

    ifstream ifs (path.string ().c_str ());
    if (!ifs.is_open ())
    {
      e << path << ": error: unable to open in read mode" << endl;
      return 1;
    }

    ifs.exceptions (ifstream::failbit | ifstream::badbit);

    // Parse and generate.
    //
    parser p (include_paths);
    unique_ptr<semantics::cli_unit> unit (p.parse (ifs, path));

    // Merge documentation variables from the command line.
    //
    for (map<string, string>::const_iterator i (ops.docvar ().begin ());
         i != ops.docvar ().end ();
         ++i)
    {
      using semantics::doc;

      // Values specified in the .cli file override command line.
      //
      if (unit->lookup<doc> ("", "var: " + i->first) != 0)
        continue;

      doc& d (unit->new_node<doc> (semantics::path ("<command line>"), 0, 0));
      unit->new_edge<semantics::names> (*unit, d, "var: " + i->first);
      d.push_back (i->second);
    }

    generator g;
    g.generate (ops, *unit, path);
  }
  catch (cli::exception const& ex)
  {
    e << ex << endl;
    return 1;
  }
  catch (semantics::invalid_path const& ex)
  {
    e << "error: '" << ex.path () << "' is not a valid filesystem path"
      << endl;
    return 1;
  }
  catch (std::ios_base::failure const&)
  {
    e << file << ": error: read failure" << endl;
    return 1;
  }
  catch (parser::invalid_input const&)
  {
    // Diagnostics has already been issued by the parser.
    //
    return 1;
  }
  catch (generator::failed const&)
  {
    // Diagnostics has already been issued by the generator.
    //
    return 1;
  }
}
