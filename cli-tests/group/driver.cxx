// file      : group/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

// Test group_scanner.
//

#include <string>
#include <cassert>
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

    bool sa (m.find ('s') != string::npos);
    bool sg (m.find ('g') != string::npos);

    argv_scanner as (--argc, ++argv);
    group_scanner s (as);

    // Verify previous two args are still valid for good measure.
    //
    const char* prev_a (0);
    string prev_s;

    // Verify position.
    //
    size_t pos (0); // argv_scanner starts from 1.

    while (s.more ())
    {
      assert (pos < s.position ());
      pos = s.position ();

      s.peek ();
      assert (pos == s.position ());

      const char* a;
      if (!sa)
      {
        a = s.next ();
        cout << "'" << a << "'";
      }
      else
        s.skip ();

      if (!sg)
      {
        scanner& gs (s.group ());
        while (gs.more ())
          cout << " '" << gs.next () << "'";
      }

      if (!sa || !sg)
        cout << endl;

      if (!sa && !sg)
      {
        s.more ();

        if (prev_a != 0)
          assert (prev_a == prev_s);

        prev_a = a;
        prev_s = a;
      }
    }

    return 0;
  }
  catch (const cli::exception& e)
  {
    cerr << e << endl;
    return 1;
  }
}
