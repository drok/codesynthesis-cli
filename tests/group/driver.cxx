// file      : tests/group/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

// Test group_scanner.
//

#include <iostream>

#include "test.hxx"

using namespace std;

int
main (int argc, char* argv[])
{
  try
  {
    using namespace cli;

    // Mode flags.
    //
    // 'g' -- don't handle groups.
    // 's' -- skip arguments.
    //
    string m (argv[1]);

    argv_scanner as (--argc, ++argv);
    group_scanner s (as);

    while (s.more ())
    {
      if (m.find ('s') == string::npos)
      {
        const char* a (s.next ());
        cout << "'" << a << "'";
      }
      else
        s.skip ();

      if (m.find ('g') == string::npos)
      {
        scanner& gs (s.group ());
        while (gs.more ())
          cout << " '" << gs.next () << "'";
      }

      cout << endl;
    }

    return 0;
  }
  catch (const cli::exception& e)
  {
    cerr << e << endl;
    return 1;
  }
}
