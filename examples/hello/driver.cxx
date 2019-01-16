// file      : examples/hello/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <iostream>

#include "hello.hxx"

using namespace std;

void
usage (ostream& os)
{
  os << "usage: driver [options] <names>" << endl
     << "options:" << endl;
  options::print_usage (os);
}

int
main (int argc, char* argv[])
{
  try
  {
    int end; // End of options.
    options o (argc, argv, end);

    if (o.help ())
    {
      usage (cout);
      return 0;
    }

    if (end == argc)
    {
      cerr << "no names provided" << endl;
      usage (cerr);
      return 1;
    }

    // Print the greetings.
    //
    for (int i = end; i < argc; i++)
    {
      cout << o.greeting () << ", " << argv[i];

      for (unsigned int j = 0; j < o.exclamations (); j++)
        cout << '!';

      cout << endl;
    }
  }
  catch (const cli::exception& e)
  {
    cerr << e << endl;
    usage (cerr);
    return 1;
  }
}
